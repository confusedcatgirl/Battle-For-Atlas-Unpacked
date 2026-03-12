<quarterSize = true, sixPoint = true>

import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/environment.h"
import "shaders/modules/graphics/shadows.h"
import "shaders/modules/graphics/lights.h"
import "shaders/modules/graphics/particles/spherical.h"
import "shaders/nodes/Shader/Functions/Lighting/GI.h"

// import "shaders/MRender/MR_ShaderDebug.h"

float3 GetSixPointSH(float3 position, float3 x, float3 y, float3 z, float3 pos, float3 neg, float aSphericalShadingScale)
{
	if (ourLightSphere.w > 0)
	{
		float3 distanceFromCenter = (position - ourLightSphere.xyz) * aSphericalShadingScale / ourLightSphere.w;
		float dotx = dot(distanceFromCenter, x);
		float doty = dot(distanceFromCenter, y);
		float dotz = dot(distanceFromCenter, z);

		pos.x *= GetSelfShadowingFromDot(dotx);
		neg.x *= GetSelfShadowingFromDot(-dotx);
		pos.y *= GetSelfShadowingFromDot(doty);
		neg.y *= GetSelfShadowingFromDot(-doty);
		pos.z *= GetSelfShadowingFromDot(dotz);
		neg.z *= GetSelfShadowingFromDot(-dotz);
	}

    // Add all the factors together into a single scale
    float scale = pos.x + pos.y + pos.z + neg.x + neg.y + neg.z;

    float3 gi = MR_ParticleGISamples[ourGISampleIndex][0].xyz;
// 	float3 gi = SampleGI( float3(0,1,0), position, Particle_IsIndoor() );
    gi *= scale;

    return gi;
}

float GetSixPointFactor(float3 x, float3 y, float3 z, float3 dir, float4 pos, float4 neg)
{
	float3 d = float3(dot(x, dir), dot(y, dir), dot(z, dir));
	d = normalize(d);
	float3 posd = saturate(d);
	float3 negd = saturate(-d);

	return saturate(posd.x * pos.x) + saturate(posd.y * pos.y) + saturate(posd.z * pos.z)
			+ saturate(negd.x * neg.x) + saturate(negd.y * neg.y) + saturate(negd.z * neg.z);
}

struct SixPointLightParameters
{
	float3 myPos;
	float mySoftRadius;
	float3 myPosNear;
	float3 myPosFar;
	float3 myXaxis;
	float3 myYaxis;
	float3 myZaxis;
	float4 myPositive;
	float4 myNegative;
	float mySphericalShadingScale;
};

void SixPointLightCalculateLighting(inout float3 aLightInOut, Gfx_AttenuatedEmission aEmission,
	Gfx_LightingDataStruct aLD, SixPointLightParameters someParameters)
{
	float3 toLight = normalize(aEmission.diffuseToLight);
	float lightFactor = GetSixPointFactor(someParameters.myXaxis, someParameters.myYaxis, someParameters.myZaxis, toLight, someParameters.myPositive, someParameters.myNegative);
	lightFactor *= GetSelfShadowingFactor(someParameters.myPos, toLight, someParameters.mySphericalShadingScale);
	aLightInOut += aEmission.diffuseEmission * lightFactor;
}

void SixPointLight(
	out float4 aLightOut,
	float4 aPositive,
	float4 aNegative,
	float4 aDiffuse,
	bool aUseShadows <default = true>,
	bool aUsePointLights <default = true>,
	float2 aSixPointUV,
	float aContrast <default = 1>,
	float aBacklightStrength <default = 1>,
	float aGamma <default = 1>,
	bool aNormalizeWeights,
	float4 aEmission,
	float aCombineAlpha <default = 1>,
	float aSphericalShadingScale <default = 1>,
	float2 aInstanceUV : Particle_InstanceUV,
	float3 aPosition : Particle_Position,
	float4 Particle_ShaderParams : Particle_ShaderParams,
	float4 MR_FragCoord : MR_FragCoord
)
{
	ourIsTransparent = true;

	aLightOut = vec4(0);
	aLightOut.a = aPositive.a;

#ifndef IS_ORBIS_TEMP
	if (aLightOut.a + aEmission.a * (1.0 - aCombineAlpha) <= 0.0)
	{
		discard;
		return;
	}
#endif

	float zdistance = Particle_ShaderParams.x;
	float3 pos = MR_ClipToWorld(MR_PixelToClip(MR_FragCoord.xy), zdistance);

@ifndef GFX_IS_PARTICLE_SHADOW
	float3 zdirection = normalize(MR_GetZAxis(MR_ViewConstants.cameraToWorld).xyz);
	float softRadius = Particle_ShaderParams.y * 0.5f;
	float3 pos_near = pos - zdirection * softRadius;
	float3 pos_far = pos + zdirection * softRadius;

	aPositive.xyz = pow(aPositive.xyz, vec3(aGamma));
	aNegative.xyz = pow(aNegative.xyz, vec3(aGamma));

	aPositive.xyz = saturate(lerp(vec3(1.0/6), aPositive.xyz, aContrast));
	aNegative.xyz = saturate(lerp(vec3(1.0/6), aNegative.xyz, aContrast));

	if (aNormalizeWeights)
	{
		float sum = dot(aPositive.xyz, vec3(1)) + dot(aNegative.xyz, vec3(1));
		sum = max(1.0, sum);
		aPositive.xyz /= sum;
		aNegative.xyz /= sum;
	}

	aNegative.z *= aBacklightStrength;

	float2 uvx2 = ddx(aSixPointUV);
	float2 uvy2 = ddy(aSixPointUV);
	float3 uvx = normalize(float3(uvx2, 0));
	float3 uvy = normalize(float3(uvy2, 0));
	float3 uvz = float3(0,0,1);

	float3 tx = uvx;
	float3 ty = uvy;
	float3 tz = uvz;
	uvx = float3(tx.x, -ty.x, tz.x);
	uvy = float3(-tx.y, ty.y, tz.y);
	uvz = float3(tx.z, ty.z, tz.z);

	float3 xaxis = normalize(MR_TransformNormal(MR_ViewConstants.cameraToWorld, uvx));
	float3 yaxis = normalize(MR_TransformNormal(MR_ViewConstants.cameraToWorld, uvy));
	float3 zaxis = -normalize(MR_TransformNormal(MR_ViewConstants.cameraToWorld, uvz));
    
	aLightOut.rgb += GetSixPointSH(pos, xaxis, yaxis, zaxis, aPositive.xyz, aNegative.xyz, aSphericalShadingScale);

	float3 toSunDir = MR_ParticleGISamples[ourGISampleIndex][1].xyz;
	float3 sunColor = MR_ParticleGISamples[ourGISampleIndex][2].xyz;
	float sunFactor = GetSixPointFactor(xaxis, yaxis, zaxis, toSunDir, aPositive, aNegative);
	float3 sunLight = sunColor * sunFactor;
	if (aUseShadows)
	{
		sunLight *= GetShadowFactor(pos, MR_FragCoord.xy);
		sunLight *= GetSelfShadowingFactor(pos, toSunDir, aSphericalShadingScale);
	}
	aLightOut.rgb += sunLight;

	Gfx_LightingDataStruct ld = Gfx_GetLightingDataDummy(pos, MR_FragCoord.xy, MR_FragCoord.w, Particle_IsIndoor());

	if (aUsePointLights)
	{
		SixPointLightParameters params;
		params.myPos = pos;
		params.mySoftRadius = softRadius;
		params.myPosNear = pos_near;
		params.myPosFar = pos_far;
		params.myXaxis = xaxis;
		params.myYaxis = yaxis;
		params.myZaxis = zaxis;
		params.myPositive = aPositive;
		params.myNegative = aNegative;
		params.mySphericalShadingScale = aSphericalShadingScale;

@include "shaders/modules/graphics/lights_code.h"
		CalculateLighting2(SixPointLightCalculateLighting, aLightOut.rgb, ld, params);
	}
@endif

	aLightOut *= aDiffuse;

	float4 lightOut_CombinedAlpha = aLightOut;
	lightOut_CombinedAlpha.rgb += aEmission.rgb * aEmission.a;

	float4 lightOut_SeparateAlpha = aLightOut;
	lightOut_SeparateAlpha.a = saturate(lightOut_SeparateAlpha.a + aEmission.a * 0.01);
	lightOut_SeparateAlpha.rgb += aEmission.rgb * aEmission.a / (lightOut_SeparateAlpha.a + 0.00001);

	aLightOut = lerp(lightOut_SeparateAlpha, lightOut_CombinedAlpha, aCombineAlpha);

@ifndef GFX_IS_PARTICLE_SHADOW
	const float4 fog = MR_ParticleGISamples[ourGISampleIndex][3];
	OW_Fog_ApplyExtinction( aLightOut.rgb, fog );
	@ifdef OGFX_IS_BLENDED
		OW_Fog_AddFogColor( aLightOut.rgb, fog );
	@endif // OGFX_IS_BLENDED
@endif
@ifdef OGFX_IS_BACKDROP // ONWARD: Don't use this for backdrop
	aLightOut = float4(1,0,1,1);
@endif // OGFX_IS_BACKDROP
}
