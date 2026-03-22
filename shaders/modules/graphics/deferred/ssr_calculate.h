@include "shaders/modules/graphics/deferred/ssr.h"

import "shaders/MRender/MR_ViewConstants.h"

import "shaders/modules/graphics/deferred_data.h"
import "shaders/modules/graphics/lighting_data.h"
import "shaders/modules/graphics/normal_encode.h"
import "shaders/modules/graphics/ssr_common.h"
import "shaders/modules/graphics/environment.h"

import "shaders/modules/graphics/onward/material.h"
import "shaders/nodes/Shader/Functions/Onward/Lighting/OW_MixedFog.h"

MR_Sampler2D Noise {
	texture = "textures/gaussian_rgb_uniform_a_256x256.tga",
	filter = point,
};

bool DSSR_Sample(float3 aRayPos, inout float2 aLastClip2,
	out float2 aPixelOut, out float aObstructionDepthOut, out float aCalculatedDepthOut, out float aDepthOut, out float aAlphaOut
)
{
	float2 lastClip2 = aLastClip2;

	float4 clipPos = MR_Transform(MR_ViewConstants.worldToClip, float4(aRayPos, 1));
	aCalculatedDepthOut = clipPos.w;
	aAlphaOut = 1;

	float2 clipPos2 = clipPos.xy / clipPos.w;
	aLastClip2 = clipPos2;
	if (abs(clipPos2.x) >= 1 || abs(clipPos2.y) >= 1)
		return false;

	float2 pixel = MR_ClipToPixel(clipPos2);
	float2 quv = MR_ClipToQuarterFramebufferUV(clipPos2);
	float depth = MR_SampleLod0(QuarterDepthbuffer, quv).x;
// 	float depth = MR_SampleTexelLod0(Depthbuffer, pixel).x;

@ifdef SSR_HIT_SKY
	depth = min(depth, 1900);
@else
@	ifdef SSR_STOP_AT_SKY
	if (depth >= 2000)
		return false;
@	endif
@endif

	aPixelOut = pixel;
	aObstructionDepthOut = clipPos.w - depth;
	aDepthOut = depth;

	float2 dir2 = clipPos2 - lastClip2;
	float2 distFromEdge2 = vec2(1) - (dir2 < 0 ? clipPos2 : -clipPos2);

	float distFromEdge = min(distFromEdge2.x, distFromEdge2.y);
	aAlphaOut = saturate(max(0.01, distFromEdge) * 4);

	return true;
}

@ifdef SSR_FOG

float3 DSSR_ApplyFog(float3 aLight, float3 aPixelPos, float3 aStartPos, int2 aPixel)
{
	float3 lightOut = aLight;
	OW_MixedFog(lightOut, aPixelPos, float4(aPixel + vec2(0.5), 0, 0));
	return lightOut;
}

@endif // SSR_FOG

float4 DSSR_GetReflectionSingle(
	float3 aStartPos,
	float2 aFFC,
	float aNoise,
	float aDepth,
	float3 aDirection,
	float3 aNormal,
	float aNumSamples,
	float aRoughness,
	out float aBlurRadiusOut,
	bool aSimple
)
{
	aBlurRadiusOut = 0;

	int NumSamples = int(aNumSamples);
	float firstStepSize = aRoughness > SSR.roughLimit ? SSR.roughFirstStepSize : SSR.firstStepSize;
	float stepSize = firstStepSize * (aNoise * 0.5 + 1.0);

	bool foundPixel = false;
	float2 bestPixel = vec2(-100);
	float3 lastRayPos = aStartPos;
	float3 rayPos = aStartPos + aDirection * aDepth * stepSize;

	float2 pixel = vec2(-100);
	float2 lastClip2 = vec2(0);
	float lastObstructionDepth = 10000;
	float obstructionDepth = 0;
	float sampledDepth = 10000;
	float depth = 10000;
	float stepsTaken = 0;
	float numBlocked = 0;
	float alphaFromSample = 0;
	for (int i=0; i<NumSamples; ++i)
	{
		if (!DSSR_Sample(rayPos, lastClip2, pixel, obstructionDepth, depth, sampledDepth, alphaFromSample))
			break;

		stepSize *= SSR.stepFactor;
		stepSize = min(stepSize, SSR.maxStepSize);

		if ((obstructionDepth > 0) && (obstructionDepth < stepSize * depth) && (sampledDepth < 2000))
		{
			if (aSimple)
				return vec4(1);

			bestPixel = pixel;
			foundPixel = true;
			break;
		}

@ifndef CRASH_WORKAROUND
		if (obstructionDepth > 0)
		{
			numBlocked += 1.0;

@ifdef SSR2
//			if (numBlocked > 2)
//				break;
@endif
		}
@endif

		lastRayPos = rayPos;
		lastObstructionDepth = obstructionDepth;
		rayPos += aDirection * depth * stepSize;
		stepsTaken += 1.0;
	}
	if (foundPixel && !aSimple)
	{
		float lastSampleAlpha = 1;
		if (1)
		{
			lastSampleAlpha = 0.5;
			float obstructionUncertainty = obstructionDepth - lastObstructionDepth;
			float3 newRayPos = lerp(rayPos, lastRayPos, obstructionDepth / obstructionUncertainty);

			float newObstructionDepth = 0;
			if (DSSR_Sample(newRayPos, lastClip2, pixel, newObstructionDepth, depth, sampledDepth, alphaFromSample))
			{
				if (abs(newObstructionDepth) <= abs(obstructionUncertainty) && sampledDepth < 2000)
				{
					rayPos = newRayPos;
					bestPixel = pixel;
					lastSampleAlpha = 1;
				}
			}
		}

		alphaFromSample *= lastSampleAlpha;

@ifdef SSR2
		float4 clip4 = MR_Transform(MR_ViewConstants.worldToClip, rayPos);

		return float4(clip4.xyz / clip4.w, alphaFromSample);
		
@else
		float alpha = alphaFromSample;
		float pixelDistance = length(bestPixel - aFFC);
		aBlurRadiusOut = pixelDistance * aRoughness * 0.3 / (2 /*quarter size*/);

		{
			float4 mrt0 = MR_SampleTexelLod0(MRT0, bestPixel);
			float4 mrt1 = MR_SampleTexelLod0(MRT1, bestPixel);
			float4 mrt2 = MR_SampleTexelLod0(MRT2, bestPixel);

			Gfx_DeferredData data;
			Gfx_MRTToDeferred(data, mrt0, mrt1, mrt2);
			float3 worldNormal = data.worldNormal;

			if (dot(worldNormal, aDirection) > 0)
 				return vec4(0);
		}
			
		float visibility = saturate(1.0 - numBlocked / max(1.0f, stepsTaken));

		float3 light = MR_SampleTexelLod0(Light_RGB, bestPixel).xyz;

@ifdef SSR_FOG
		light = DSSR_ApplyFog(light, rayPos, aStartPos, bestPixel);
@endif // SSR_FOG

		return float4(light * visibility, alpha);
@endif
	}

	return vec4(0);
}

void CalculateSSR(out float3 colorOut, out float radiusOut, out float alphaOut, out float signOut, float2 ffc, float2 qfc)
{
	float4 mrt0 = MR_SampleTexelLod0(MRT0, ffc);
	float4 mrt1 = float4(MR_SampleTexelLod0(Normal, qfc), 0);
	float4 mrt2 = MR_SampleTexelLod0(MRT2, ffc);

@ifdef SSS
	float depth = MR_SampleTexelLod0(Depthbuffer, ffc);
@else
	float depth = MR_SampleTexelLod0(Depth, qfc);
@endif

	Gfx_DeferredData data;
	Gfx_MRTToDeferred(data, mrt0, mrt1, mrt2);

	const float roughness = OW_Material_ArtisticToRoughnessUnclamped(data.roughness);
	const float invRoughness = 1.0 - roughness;

	const float roughnessSquared = roughness * roughness;
	float invRoughnessSquared = 1.0 - roughnessSquared;

	colorOut = vec3(0);
	radiusOut = 0;
	alphaOut = 0;
	signOut = 0;

@ifdef SSS
	if (data.sss)
	{
		radiusOut = data.sss * SSR.sssScale / depth;
		if (radiusOut > 1)
		{
			alphaOut = saturate(radiusOut - 1);
			colorOut = MR_SampleTexelLod0(OldLight_RGB, ffc).xyz;
			signOut = -1;
		}
		return;
	}
@endif // SSS

@ifdef SSR

	if (roughness > SSR.maxRoughness || depth > SSR.range)
	{
		return;
	}

	float2 clip2 = MR_PixelToClip(ffc);
	float3 pos = MR_ClipToWorld(clip2, depth);

	Gfx_LightingDataStruct ld = Gfx_GetLightingData(pos, float4(ffc, 0, depth));

	float3 normal = data.worldNormal;

	/* @ifdef SSR_FLATTEN_GROUND
	if (normal.y > SSR.flattenGroundThreshold)
		normal = float3(0,1,0);
	@endif */

	float4 reflectedLight = vec4(0);
	float reflectionAlpha = 0;
	float blurRadius = 0;

	float3 vertexToCamera = normalize(ld.toCamera);

	float numSamples = SSR.numSamples * invRoughnessSquared;

@ifdef CRASH_WORKAROUND
@	define SSR_SINGLE_RAY
@endif // CRASH_WORKAROUND

@ifdef SSR_SINGLE_RAY
	numSamples = (roughness < SSR.roughLimit ? SSR.numSamples : SSR.roughNumSamples) * invRoughnessSquared;
	float numRays = 1;
	float samplesPerRay = numSamples;

	{
		float2 uv = qfc.xy / 256;
@else
	float numRays = max(1.0, ceil(roughness * SSR.maxRays));
	float samplesPerRay = floor(numSamples / numRays);

	for (int i=0; i<numRays; ++i)
	{
		float2 uv = (qfc.xy + i*int2(173,13)) / 256;

@endif // SSR_SINGLE_RAY

		float4 noise = MR_SampleLod0(Noise, uv + SSR.jitter);

		float3 offset = vec3(0);

@ifndef SSR_SINGLE_RAY
		if (i)
		{
			offset = noise.xyz - vec3(0.5);
			offset *= 25.0 / pow(1000.0, invRoughness);
			offset *= saturate(roughness * 128.0);
		}
@endif // SSR_SINGLE_RAY

		float3 offsetedNormal = normal + offset;
		float3 reflected = reflect(-vertexToCamera, offsetedNormal);
		reflected = OW_Material_GetDominantSpecDir(reflected, offsetedNormal, roughness, roughnessSquared);

		if (dot(reflected, normal) > 0)
		{
			float tempBlurRadius;
			float4 ssr = DSSR_GetReflectionSingle(ld.worldPosition, ffc, noise.w, ld.depth,
				normalize(reflected), normal, samplesPerRay, roughness, tempBlurRadius, false);

			reflectedLight += float4(ssr.xyz, 1) * ssr.w;
			reflectionAlpha += ssr.a / numRays;
			blurRadius += tempBlurRadius / numRays;
		}
	}

	reflectionAlpha *= saturate(1 - pow(roughness / SSR.maxRoughness, 5));

/*
	if (1)
	{
		float maxIntensity = Gfx_Environment.averageLuminosity;

		// clamp intensity
		float intensity = length(reflectedLight);
		reflectedLight = reflectedLight * min(intensity, maxIntensity) / max(0.0001, intensity);

		if (intensity > maxIntensity)
		{
			alphaOut *= maxIntensity / intensity;
		}
	}
*/

	blurRadius += 2;

	colorOut = reflectedLight.xyz / max(0.001, reflectedLight.w);
	radiusOut = blurRadius;
	alphaOut = reflectionAlpha;
	signOut = 1;
@endif
}
