
import "shaders/nodes/Shader/Functions/Lighting/Environment Map.h"
import "shaders/nodes/Shader/Functions/Fresnel-Schlick.h"

import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/modules/graphics/onworld/onworld_environment.h"

import "shaders/modules/graphics/onworld/onworld_brdf.h"

import "shaders/modules/graphics/onworld/sun_light.h"
@include "shaders/modules/graphics/onworld/onworld_deferred_lighting.h"

void OW_FullBaseSpaceLighting(

	out float3 aLightOut,

	float3 aAlbedo,
	float3 aNormal,

	float aRoughness <default = 1>,
	float aMetalness <default = 0>,

	float aAmbientOcclusion <default = 1>,

	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData
)
{
	Gfx_LightingData.isUnflatten = true;

	OW_SunLitPixel sunLitPixel;
	OW_InitPositionContext( sunLitPixel.myPosCtx, Gfx_LightingData.worldPosition, OW_Planet_GetCurrentGroundSphere( ), true, Gfx_LightingData.isUnflatten );
	OW_Lighting_InitSunLitPixel( sunLitPixel.mySunCtx, sunLitPixel.myPosCtx, normalize(aNormal) );

	float3 ignoredOut[3];
	aLightOut = OW_DeferredLighting( vec4(1.0f), // debug

		sunLitPixel,

		aAlbedo,
		aNormal, // not used

		aRoughness,
		aMetalness,

		aAmbientOcclusion,

		1.0,
		0.0,

		Gfx_LightingData,

		ignoredOut[0],
		ignoredOut[1],
		ignoredOut[2]);
}

void OW_FullBaseSpaceLighting(

	out float3 aLightOut,

	float3 aAlbedo,
	float3 aNormal,

	float aOpacity <default = 1>,

	float aRoughness <default = 1>,
	float aMetalness <default = 0>,

	float aAmbientOcclusion <default = 1>,

	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData
)
{
	Gfx_LightingData.isUnflatten = true;

	OW_SunLitPixel sunLitPixel;
	OW_InitPositionContext( sunLitPixel.myPosCtx, Gfx_LightingData.worldPosition, OW_Planet_GetCurrentGroundSphere( ), true, Gfx_LightingData.isUnflatten );
	OW_Lighting_InitSunLitPixel( sunLitPixel.mySunCtx, sunLitPixel.myPosCtx, normalize(aNormal) );

	float3 ignoredOut[3];
	aLightOut = OW_DeferredLighting( vec4(1.0), // debug

		sunLitPixel,

		aAlbedo,
		aNormal, // not used

		aRoughness,
		aMetalness,

		aAmbientOcclusion,

		@ifdef HAS_OPACITY
			aOpacity,
		@else
			1.0,
		@endif // HAS_OPACITY
		0.0,

		Gfx_LightingData,

		ignoredOut[0],
		ignoredOut[1],
		ignoredOut[2]);
}