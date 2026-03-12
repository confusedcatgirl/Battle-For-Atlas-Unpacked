import "shaders/modules/graphics/onward/lighting/brdf.h"

MR_Buffer<uint4> SamplePatterns : MR_Buffer0;

float SpecWeight(float3 aN, float3 aL, float3 aV, float aRoughness)
{
	float3 H = normalize(aL + aV);
	float dotNH = saturate(dot(aN, H));
	float dotLH = saturate(dot(aL, H));

	return saturate(OW_SpecBRDF_Distribution_1(aRoughness, dotNH) * dotNH / max(4.0 * dotLH, 0.00001));
}

float4 GetWeightedHit(inout float weightAccum, inout float3 toHitAccum, Gfx_DeferredData data, float roughness, float3 worldPos, float3 toCameraDir, float2 clip2, int2 hitpx, float weight, float depth)
{
@ifdef SSR2_QUARTER_RAYS
	float4 hit = Hits[min(hitpx, SSR.quarterSourceMax)];
@else
	float4 hit = Hits[min(hitpx, MR_ViewConstants.viewportMax)];
@endif // SSR2_QUARTER_RAYS

	if (!hit.w)
	{
		return vec4(0);
	}

	float lightLod = GetLightLod(roughness, float3(clip2, 0), float3(hit.xy, 0));
	float3 light = SampleLight(hit.xy, lightLod);

	float4 worldHit4 = MR_Transform(MR_ViewConstants.clipToWorld, hit.xyz);
	float3 worldHit = worldHit4.xyz / worldHit4.w;

	float3 toHit = worldHit - worldPos;

	float4 ret = float4(light, hit.w);

@ifdef SSR2_MULTISAMPLE_RESOLVE

	float distanceToHit = length(toHit);
	// change behavior of roughness and behavior near ray hit position
	float weightingRoughness = lerp(0.2, max(0.05, roughness), saturate(distanceToHit * 10.0 / depth));
	weight *= SpecWeight(data.worldNormal, toHit * rcp(distanceToHit), toCameraDir, weightingRoughness);
	ret *= weight;

@endif // SSR2_MULTISAMPLE_RESOLVE

	toHitAccum += toHit * weight;
	weightAccum += weight;

	return ret;
}

float4 FogResult(in float4 aResult, float3 aWorldPos, float3 aToHit, int2 aPixel)
{
@ifdef SSR_FOG
	aResult.xyz = DSSR_ApplyFog(aResult.xyz, aWorldPos + aToHit, aWorldPos, aPixel);
@endif // SSR_FOG
	return aResult;
}

float4 GetWeightedHits(inout float distanceOut, Gfx_DeferredData data, float roughness, float depth, float3 worldPos, float3 toCameraDir, float2 clip2, int2 px, int2 hitpx)
{
	float weightAccum = 0;

	int2 offsetFudge = 0;

@ifdef SSR2_QUARTER_RAYS
	offsetFudge.x += int(px.x & 1) - int (SSR.frameIndex & 1);
	offsetFudge.y += int(px.y & 1) - int((SSR.frameIndex & 2) >> 1);
@endif // SSR2_QUARTER_RAYS

@ifdef SSR2_MULTISAMPLE_RESOLVE

	if (data.roughness <= 0.05) // improve "mirror like" reflections
	{
		float3 toHit = vec3(0);

		@ifdef SSR2_QUARTER_RAYS
			hitpx += offsetFudge;
		@endif // SSR2_QUARTER_RAYS

		float4 ret = GetWeightedHit(weightAccum, toHit, data, roughness, worldPos, toCameraDir, clip2, hitpx, 1, depth);
		if (ret.w > 0)
		{
			distanceOut = length(toHit);
			return FogResult(ret, worldPos, toHit, px);
		}
	}

	float3 toHitAccum = vec3(0);
	float4 sum = vec4(0);

	float noise = Noise[px & 255].x;
	uint index = (uint(noise * 255) + uint(SSR.frameIndex)) % uint(7);

	uint4 offset = SamplePatterns[index];
	int2 spx = hitpx - int2(3,3);

	sum += GetWeightedHit(weightAccum, toHitAccum, data, roughness, worldPos, toCameraDir, clip2, spx + int2(offset.x, offset.z), 1, depth);
	sum += GetWeightedHit(weightAccum, toHitAccum, data, roughness, worldPos, toCameraDir, clip2, spx + int2(offset.x, offset.w), 1, depth);
	sum += GetWeightedHit(weightAccum, toHitAccum, data, roughness, worldPos, toCameraDir, clip2, spx + int2(offset.y, offset.z), 1, depth);
	sum += GetWeightedHit(weightAccum, toHitAccum, data, roughness, worldPos, toCameraDir, clip2, spx + int2(offset.y, offset.w), 1, depth);

	float invWeightUnclamped = 1 / max(weightAccum, 0.00001);

	float3 toHit = toHitAccum * invWeightUnclamped;
	distanceOut = length(toHit);

	// accept worse fits near the reflection point since rays will necessarily not hit straight on,
	// but require more hits at a distance to avoid noise
	float minWeight = lerp(0.5, 4.0, saturate(distanceOut * 2 / depth));
	float4 ret = float4(sum.xyz * invWeightUnclamped, sum.w / max(minWeight, weightAccum));

@else

	@ifdef SSR2_QUARTER_RAYS
		hitpx += offsetFudge;
	@endif // SSR2_QUARTER_RAYS

	float3 toHit = vec3(0);
	float4 ret = GetWeightedHit(weightAccum, toHit, data, roughness, worldPos, toCameraDir, clip2, hitpx, 1, depth);
	distanceOut = length(toHit);

@endif // SSR2_MULTISAMPLE_RESOLVE

@ifdef SSR_FOG
	if (ret.w > 0)
	{
		ret = FogResult(ret, worldPos, toHit, px);
	}
@endif // SSR_FOG

	return ret;
}

void ResolveSSR(out float3 colorOut, out float alphaOut, out float distanceOut, float2 ffc, uint2 hitpx)
{
	colorOut = vec3(0);
	alphaOut = 0;
	distanceOut = 0;

	int2 px = int2(ffc.xy);

	float4 mrt0 = MR_SampleTexelLod0(MRT0, px);
	float4 mrt1 = float4(MR_SampleTexelLod0(Normal, px), 0);
	float4 mrt2 = MR_SampleTexelLod0(MRT2, px);

	float depth = MR_SampleLinearDepth(px);

	Gfx_DeferredData data;
	Gfx_MRTToDeferred(data, mrt0, mrt1, mrt2);
	const float roughness = OW_Material_ArtisticToRoughnessUnclamped(data.roughness);

	float2 clip2 = MR_PixelToClip(ffc);
	float3 camSpacePos = MR_ClipToCamera(clip2, depth);
	float3 worldPos = MR_Transform(MR_ViewConstants.cameraToWorld, camSpacePos).xyz;

	float3 toCameraDir = normalize(MR_GetPos(MR_ViewConstants.cameraToWorld).xyz - worldPos);

	float4 hitSample = GetWeightedHits(distanceOut, data, roughness, depth, worldPos, toCameraDir, clip2, px, hitpx);

	colorOut = hitSample.xyz;
	alphaOut = hitSample.w;
}
