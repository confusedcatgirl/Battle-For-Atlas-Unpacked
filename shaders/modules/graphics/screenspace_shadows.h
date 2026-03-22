@ifndef GFX_NO_SHADOWS

@ifdef GFX_CINEMATIC_SHADERS
@	define GFX_USE_SS_SHADOWS
@	define GFX_USE_SS_SHADOWS_HQ
@	define GFX_SS_SHADOW_SPOT_LIGHTS
@else
@	ifndef GFX_NO_SS_SHADOWS
@		define GFX_USE_SS_SHADOWS
@		ifndef GFX_SS_SHADOW_LOW_QUALITY
@			define GFX_USE_SS_SHADOWS_HQ
@		endif
@	endif
@endif

@ifdef GFX_USE_SS_SHADOWS
import "shaders/modules/graphics/ssr_common.h"
import "shaders/modules/graphics/shadows_common.h"

// TODO: this whole thing really needs a rewrite in screen/z space

bool SSSh_Sample4(float3 aRayPos, out float4 aDepthDiffOut, out float aDepthOut)
{
	float4 clipPos = MR_Transform(MR_ViewConstants.worldToClip, float4(aRayPos, 1));
	aDepthOut = clipPos.w;

	[flatten]
	if (max(abs(clipPos.x / clipPos.w), abs(clipPos.y / clipPos.w)) < 1)
	{
		float2 uv = MR_ClipToFramebufferUV(clipPos.xy / clipPos.w);
		float4 depths = MR_SampleGather(Depthbuffer, uv);
		aDepthDiffOut = depths - vec4(clipPos.w);

		return true;
	}
	else
		return false;
}

float SSSh_GetShadow4(
	float3 aStartPos,
	float3 aDirection,
	float aMaxDistance,
	uint aNumSamples,
	float aDepthTolerance,
	float aSampleOffset
)
{
	float stepSize = aMaxDistance / aNumSamples;

	float3 lastRayPos = aStartPos;
	float3 rayPos = aStartPos + aDirection * stepSize * aSampleOffset;

	bool4 visible = bool4(true,true,true,true);
	float4 depthDiff;
	for (uint i=0; i<aNumSamples; ++i)
	{
		float depth;
		if (!SSSh_Sample4(rayPos, depthDiff, depth))
			break;

		float epsilon = depth * 0.0008;

		bool4 sampleShadowed = depthDiff < -epsilon && depthDiff > vec4(-aDepthTolerance + epsilon);
		visible = visible && !sampleShadowed;

		lastRayPos = rayPos;
		rayPos += aDirection * stepSize;
	}
	return (float(visible.x) + float(visible.y) + float(visible.z) + float(visible.w)) * 0.25;
}
@endif
@endif

void SSSh_FriendlyShadow(
	inout float3 aShadowInOut,
	float3 aToLight,
	float aMaxDepth <default = 10>,
	float aDepthTolerance,
	float aMaxRayDistance,
	uint aNumRaySteps,
	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData
)
{
@ifdef GFX_USE_SS_SHADOWS
	if (ourForceDisableScreenspaceShadows || aShadowInOut.x < 0.001 || Gfx_LightingData.depth > aMaxDepth)
		return;

	float3 direction = normalize(aToLight);
	float maxRayDistance = min(aMaxRayDistance, length(aToLight));

	float2 noise = GetRotateVec(Gfx_LightingData.pixelPosition) * 0.5;
	float offset = 1.0 + noise.x;

	float sum = 0.0;
@ifndef GFX_USE_SS_SHADOWS_HQ
	sum += SSSh_GetShadow4(Gfx_LightingData.worldPosition, direction, maxRayDistance, aNumRaySteps, aDepthTolerance, offset);
@else
	int nsupersamples = 4;
	float fnsupersamples = nsupersamples;

	sum += SSSh_GetShadow4(Gfx_LightingData.worldPosition, direction, maxRayDistance, aNumRaySteps, aDepthTolerance, offset);
	sum += SSSh_GetShadow4(Gfx_LightingData.worldPosition, direction, maxRayDistance, aNumRaySteps, aDepthTolerance, offset * (fnsupersamples - 1)/fnsupersamples);
	if (sum == 0.0 || sum == 2.0)
		sum /= 2.0;
	else
	{
		for (int i=1; i<nsupersamples-1; ++i)
			sum += SSSh_GetShadow4(Gfx_LightingData.worldPosition, direction, maxRayDistance, aNumRaySteps, aDepthTolerance, offset * i/fnsupersamples);
		sum /= fnsupersamples;
	}
@endif

	aShadowInOut *= lerp(sum, 1.0, saturate((Gfx_LightingData.depth / aMaxDepth - 0.8) * 5.0));
@endif
}

void SSSh_FriendlyShadow(
	inout float3 aShadowInOut,
	float3 aToLight,
	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData
)
{
@ifdef GFX_USE_SS_SHADOWS

	uint nsamples = 4;
@ifndef GFX_USE_SS_SHADOWS_HQ
	nsamples = 3;
@endif

	if (Gfx_LightingData.depth > 6)
		nsamples = nsamples >> 1;

	SSSh_FriendlyShadow(aShadowInOut, aToLight, 12, 0.03, 0.04, nsamples, Gfx_LightingData);
@endif
}

void SSSh_FriendlySpotShadow(
	inout float3 aShadowInOut,
	float3 aToLight,
	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData
)
{
@ifndef GFX_NO_SHADOWS
@ifdef GFX_SS_SHADOW_SPOT_LIGHTS

	uint nsamples = 4;
@ifndef GFX_USE_SS_SHADOWS_HQ
	nsamples = 3;
@endif

	if (Gfx_LightingData.depth > 6)
		nsamples = nsamples >> 1;

	float dist = length(aToLight);
	float maxRange = 12;
	float range = min(dist * 0.8, maxRange);

	if (range > 0.2)
		SSSh_FriendlyShadow(aShadowInOut, aToLight / dist, range, 0.03, 0.04, nsamples, Gfx_LightingData);
@endif
@endif
}
