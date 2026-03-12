import "shaders/modules/graphics/global_variables.h"
import "shaders/modules/graphics/environment_constants.h"
import "shaders/modules/graphics/shadows_common.h"
import "shaders/modules/graphics/spot_shadows.h"
import "shaders/modules/graphics/shadow_functions.h"

@ifndef GFX_NO_SHADOWS

import "shaders/MRender/MR_ViewConstants.h"

@ifdef PARALLAX_SHADOWS
global float ourParallaxShadowFactor = 1.0;
@endif // PARALLAX_SHADOWS

@ifdef CSM_DEBUGCOLOR
float3 CalculateCSMDebugColor( float3 aWorldPos, float2 aScreenSpaceCoord )
{
	const float3 min_slice = float3(0.0, 0.0, 0.0);
	const float3 max_slice = float3(1.0, 1.0, 1.0);

@ifdef GFX_THREE_SHADOW_CASCADES

	float4 slice_pos_2 = MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[2], aWorldPos);
	float4 shadowmap_pos = slice_pos_2;
	const float3 midPos = float3(0.5, 0.5, 0.5);
	float finalCascadeDist = max(max(abs(shadowmap_pos.x - midPos.x), abs(shadowmap_pos.y - midPos.y)), midPos.z - shadowmap_pos.w);
	float3 sliceTemp;

@else

	float4 slice_pos_3 = MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[3], aWorldPos);
	float4 shadowmap_pos = slice_pos_3;
	const float3 midPos = float3(0.5, 0.5, 0.5);
	float finalCascadeDist = max(max(abs(shadowmap_pos.x - midPos.x), abs(shadowmap_pos.y - midPos.y)), midPos.z - shadowmap_pos.w);

	float4 slice_pos_2 = MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[2], aWorldPos);
	float3 sliceTemp = step(min_slice, slice_pos_2.xyw) * step(slice_pos_2.xyw, max_slice);
	float slice_mask_2 = sliceTemp.x * sliceTemp.y * sliceTemp.z;
	shadowmap_pos = lerp(shadowmap_pos, slice_pos_2, slice_mask_2);

@endif // GFX_THREE_SHADOW_CASCADES

	float4 slice_pos_1 = MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[1], aWorldPos);
	sliceTemp = step(min_slice, slice_pos_1.xyw) * step(slice_pos_1.xyw, max_slice);
	float slice_mask_1 = sliceTemp.x * sliceTemp.y * sliceTemp.z;
	shadowmap_pos = lerp(shadowmap_pos, slice_pos_1, slice_mask_1);

	float4 slice_pos_0 = MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[0], aWorldPos);
	sliceTemp = step(min_slice, slice_pos_0.xyw) * step(slice_pos_0.xyw, max_slice);
	float slice_mask_0 = sliceTemp.x * sliceTemp.y * sliceTemp.z;
	shadowmap_pos = lerp(shadowmap_pos, slice_pos_0, slice_mask_0.x);

@ifdef GFX_THREE_SHADOW_CASCADES
	float useFinalCascade = (1-slice_mask_0) * (1-slice_mask_1);
@else
	float useFinalCascade = (1-slice_mask_0) * (1-slice_mask_1) * (1-slice_mask_2);
@endif // GFX_THREE_SHADOW_CASCADES
	float shadowFade = useFinalCascade*smoothstep(0.4, 0.5, finalCascadeDist);

@ifdef GFX_NVIDIA_SHADOWS
	shadowmap_pos.w = ( ShadowConstants.NVIDIAShadowSampleType == ourNVIDIASampleMaps ) ? ( 1.0f - shadowmap_pos.w ) : ( shadowmap_pos.w );
@endif // GFX_NVIDIA_SHADOWS

	float3 color = 0.0f;
	if (shadowmap_pos.z < 1.0f)
		color = float3(1.0f, 0.0f, 0.0f);
	else if (shadowmap_pos.z < 2.0f)
		color = float3(0.0f, 1.0f, 0.0f);
	else if (shadowmap_pos.z < 3.0f)
		color = float3(1.0f, 0.0f, 1.0f);
	else if (shadowmap_pos.z < 4.0f)
		color = float3(0.0f, 0.0f, 1.0f);
	return  color;
}
@endif // CSM_DEBUGCOLOR

float3 GetShadowCSM( float4 aShadowPos, float2 aScreenSpaceCoord, float shadowFade )
{
	float2 rotateVec = GetRotateVec(aScreenSpaceCoord);
	float shadowFactor = CalcShadowFactorArray(ShadowMap, aShadowPos, rotateVec, ShadowConstants.filterOffset);

@ifdef GFX_NVIDIA_SHADOWS
	shadowFactor = ( ShadowConstants.NVIDIAShadowSampleType == ourNVIDIASampleMaps ) ? ( 1.0f - shadowFactor ) : ( shadowFactor );
@endif // GFX_NVIDIA_SHADOWS

	return vec3(lerp(shadowFactor, Gfx_Environment.defaultShadowFactor, shadowFade));
}

float3 GetShadowFactor( float3 aWorldPos, float2 aScreenSpaceCoord )
{
	const float3 min_slice = float3(0.0, 0.0, 0.0);
	const float3 max_slice = float3(1.0, 1.0, 1.0);

@ifdef GFX_THREE_SHADOW_CASCADES

	float4 slice_pos_2 = MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[2], aWorldPos);
	float4 shadowmap_pos = slice_pos_2;
	const float3 midPos = float3(0.5, 0.5, 0.5);
	float finalCascadeDist = max(max(abs(shadowmap_pos.x - midPos.x), abs(shadowmap_pos.y - midPos.y)), midPos.z - shadowmap_pos.w);
	float3 sliceTemp;

@else

	float4 slice_pos_3 = MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[3], aWorldPos);
	float4 shadowmap_pos = slice_pos_3;
	const float3 midPos = float3(0.5, 0.5, 0.5);
	float finalCascadeDist = max(max(abs(shadowmap_pos.x - midPos.x), abs(shadowmap_pos.y - midPos.y)), midPos.z - shadowmap_pos.w);

	float4 slice_pos_2 = MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[2], aWorldPos);
	float3 sliceTemp = step(min_slice, slice_pos_2.xyw) * step(slice_pos_2.xyw, max_slice);
	float slice_mask_2 = sliceTemp.x * sliceTemp.y * sliceTemp.z;
	shadowmap_pos = lerp(shadowmap_pos, slice_pos_2, slice_mask_2);

@endif // GFX_THREE_SHADOW_CASCADES

	float4 slice_pos_1 = MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[1], aWorldPos);
	sliceTemp = step(min_slice, slice_pos_1.xyw) * step(slice_pos_1.xyw, max_slice);
	float slice_mask_1 = sliceTemp.x * sliceTemp.y * sliceTemp.z;
	shadowmap_pos = lerp(shadowmap_pos, slice_pos_1, slice_mask_1);

	float4 slice_pos_0 = MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[0], aWorldPos);
	sliceTemp = step(min_slice, slice_pos_0.xyw) * step(slice_pos_0.xyw, max_slice);
	float slice_mask_0 = sliceTemp.x * sliceTemp.y * sliceTemp.z;
	shadowmap_pos = lerp(shadowmap_pos, slice_pos_0, slice_mask_0.x);

@ifdef GFX_THREE_SHADOW_CASCADES
	float useFinalCascade = (1-slice_mask_0) * (1-slice_mask_1);
@else
	float useFinalCascade = (1-slice_mask_0) * (1-slice_mask_1) * (1-slice_mask_2);
@endif // GFX_THREE_SHADOW_CASCADES

	float shadowFade = useFinalCascade*smoothstep(0.4, 0.5, finalCascadeDist);

@ifdef GFX_NVIDIA_SHADOWS
	shadowmap_pos.w = ( ShadowConstants.NVIDIAShadowSampleType == ourNVIDIASampleMaps ) ? ( 1.0f - shadowmap_pos.w ) : ( shadowmap_pos.w );
@endif

	return GetShadowCSM(shadowmap_pos, aScreenSpaceCoord, shadowFade);
}

float3 GetShadowFactor(float3 aWorldSpacePos, float2 aScreenSpaceCoord, float aShadowMapZOffset)
{
	float3 shadowFactor;

@ifdef GFX_NVIDIA_SHADOWS
	if( ShadowConstants.NVIDIAShadowSampleType == ourNVIDIASampleMask )
	{
		float2 uv = MR_PixelToFramebufferUV( float2( aScreenSpaceCoord.x + 0.5f, aScreenSpaceCoord.y + 0.5f ) );
		shadowFactor = MR_SampleLod0( NVIDIAShadowMask, uv ).xxx;
	}
	else
@endif
	{
		shadowFactor = GetShadowFactor(aWorldSpacePos, aScreenSpaceCoord);
	}

	return shadowFactor;
}

@else

float3 GetShadowFactor( float3 aWorldPos, float2 aScreenSpaceCoord )
{
	return vec3(1.0);
}

float3 GetShadowFactor(float3 aWorldSpacePos, float2 aScreenSpaceCoord, float aShadowMapZOffset)
{
	return vec3(1.0);
}

@endif
