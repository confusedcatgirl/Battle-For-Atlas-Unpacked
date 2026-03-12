@include "shaders/modules/graphics/deferred/ssr.h"

import "shaders/modules/graphics/shadows.h"

import "shaders/nodes/Shader/Functions/Lighting/Environment Map.h"
import "shaders/nodes/Shader/Functions/Fresnel-Schlick.h"

import "shaders/modules/graphics/onworld/planet_base.h"

import "shaders/modules/graphics/onworld/onworld_environment.h"

import "shaders/modules/graphics/onworld/onworld_brdf.h"

import "shaders/modules/graphics/onworld/onworld_deferred_lighting.h"

constants : MR_ConstantBuffer3
{
@ifdef USE_DEFERRED_DEBUG_FLAGS

	float4 DebugFlags_Lighting;

	float  DebugFlag_GrayColor;
	float  DebugFlag_ClipDepth;
	float  DebugFlag_NANInfNeg;

@else

	float4 _DebugFlags_Lighting;

	float  _DebugFlag_GrayColor;
	float  _DebugFlag_ClipDepth;
	float  _DebugFlag_NANInfNeg;

@endif // USE_DEFERRED_DEBUG_FLAGS
}

@ifndef USE_DEFERRED_DEBUG_FLAGS

	const float4 DebugFlags_Lighting = vec4(1.0);

	const float DebugFlag_GrayColor = 0.0;
	const float DebugFlag_ClipDepth = 0.0;
	const float DebugFlag_NANInfNeg = 0.0;

@endif // !USE_DEFERRED_DEBUG_FLAGS

void DeferredComplete(inout OW_SunLitPixel aSunLitPixel, in Gfx_LightingDataStruct aLightingData, out float3 colorOut, out float3 ssParamOut0, out float3 ssParamOut1, out float3 ssParamOutN, int2 pixel)
{
	float4 mrt0 = MR_SampleTexelLod0(MRT0, pixel);

@ifdef USE_DEFERRED_DEBUG_FLAGS
	mrt0.rgb = lerp( mrt0.rgb, MR_GRAY_ALBEDO, DebugFlag_GrayColor );
@endif // USE_DEFERRED_DEBUG_FLAGS

	float4 mrt1 = MR_SampleTexelLod0(MRT1, pixel);
	float4 mrt2 = MR_SampleTexelLod0(MRT2, pixel);

	Gfx_DeferredData data;
	Gfx_MRTToDeferred(data, mrt0, mrt1, mrt2);

	OW_Lighting_InitSunLitPixel(aSunLitPixel.mySunCtx, aSunLitPixel.myPosCtx, data.worldNormal);

	float3 diffOut;
	float3 reflOut;
	float3 normOut;

	colorOut = OW_DeferredLighting(

		DebugFlags_Lighting,

		aSunLitPixel,

		data.color,
		data.worldNormal,
		data.roughness,
		data.metalness,
		data.ao,
		data.opacity,
		data.minReflection,

		aLightingData,

		diffOut,
		reflOut,
		normOut);

	@ifndef SSR_AND_SSS
		ssParamOut1 = 0;
		@ifdef SSR_OR_SSS
			@ifdef SSR
				ssParamOut0 = reflOut;
				ssParamOutN = Gfx_EncodeNormal_UNorm(normOut);
			@endif // SSR
			@ifdef SSS
				ssParamOut0 = diffOut;
				ssParamOutN = 0;
			@endif // SSS
		@else
			ssParamOut0 = 0;
			ssParamOutN = 0;
		@endif // SSR_OR_SSS
	@else
		ssParamOut0 = reflOut;
		ssParamOut1 = diffOut;
		ssParamOutN = Gfx_EncodeNormal_UNorm(normOut);
	@endif // SSR_AND_SSS
}
