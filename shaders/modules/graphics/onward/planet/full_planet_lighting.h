import "shaders/nodes/Shader/Functions/Lighting/Environment Map.h"
import "shaders/nodes/Shader/Functions/Fresnel-Schlick.h"

import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/modules/graphics/onworld/onworld_environment.h"

import "shaders/modules/graphics/onworld/onworld_brdf.h"

import "shaders/modules/graphics/onworld/sun_light.h"
import "shaders/modules/graphics/onworld/onworld_deferred_lighting.h"

import "shaders/modules/graphics/onworld/atmosphere/planet_sky.h"

import "shaders/nodes/Shader/Functions/Onward/Lighting/OW_GetMixedFog.h"

@ifdef IS_SIMPLIFIED

float4 OW_Planet_GetFarAtmosphereColor(in float3 aColor, in float3 aWorldPos)
{
	const float3 eyePos = OW_Planet_GetCameraPos( );

	float3 cameraPos = MR_GetPos(MR_ViewConstants.cameraToWorld).xyz;
	const float3 eyeDir = normalize(aWorldPos - cameraPos);
	const float3 sunDir = OW_Sun_GetPlanetToSunDir( );

	float3 sunIntensity = MR_OnworldEnvironmentConstants.inSpaceSunLightIntensity;

	// since the atmosphere isn't rendered independently but on the planet, we must adjust the radius
	const float atmosphereRadius = OW_Planet_GetBaseGroundRadius( ) * 1.005f /* 6420.0f / 6360.0f */;
	return AtmosphereRimColor(eyePos, eyeDir, sunDir, sunIntensity, atmosphereRadius);
}

@endif // IS_SIMPLIFIED

void OW_FullPlanetLighting(
	out float3 aLightOut,
	float3 aNormal, float3 aDiffuse, float aRoughness, float aAmbientOcclusion, float3 aEmission,
	Gfx_DeferredData aDeferredData,
	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData
)
{
	Gfx_LightingData.isUnflatten = true;

	OW_SunLitPixel sunLitPixel;
	OW_InitPositionContext( sunLitPixel.myPosCtx, Gfx_LightingData.worldPosition, OW_Planet_GetCurrentGroundSphere( ), false, Gfx_LightingData.isUnflatten );
	OW_Lighting_InitSunLitPixel( sunLitPixel.mySunCtx, sunLitPixel.myPosCtx, aNormal );

	float3 ignoredOut[3];
	aLightOut = OW_DeferredLighting(
		vec4(1.0f),
		sunLitPixel,
		aDeferredData.color,
		aDeferredData.worldNormal,
		aDeferredData.roughness,
		aDeferredData.metalness,
		aAmbientOcclusion,
		1.0,
		0.0,
		Gfx_LightingData,
		ignoredOut[0],
		ignoredOut[1],
		ignoredOut[2]);

	float4 fog = OW_GetMixedFog( Gfx_LightingData.worldPosition, Gfx_LightingData.pixelPosition );
	
@ifdef IS_SIMPLIFIED

	aLightOut *= OW_FAKE_PLANET_TRANSMITTANCE_FROM_SPACE;

	float4 atmosphereColor = OW_Planet_GetFarAtmosphereColor( aLightOut.rgb, Gfx_LightingData.worldPosition );

	{
		float atmosphereBrightness = length(atmosphereColor.rgb);

		const float3 relCameraPos = OW_Planet_GetCameraPos();
		float4 cloudsColor = OW_Sky_GetCloudsColorFromSpace(relCameraPos, normalize( sunLitPixel.myPosCtx.mySphereRelativePos - relCameraPos ),
			sunLitPixel.myPosCtx.mySphereAbsolutePos,
			sunLitPixel.myPosCtx.mySphereRelativePos,
			atmosphereBrightness);

		aLightOut = aLightOut * (1.0f - cloudsColor.a) + cloudsColor.rgb * cloudsColor.a;

		aLightOut.rgb += atmosphereColor.rgb;
	}
@endif // IS_SIMPLIFIED

	
	OW_Fog_Apply( aLightOut, fog );

	{
		// Hack for the sun... don't apply extinction on emissive when on the ground
		fog.w = lerp( fog.w, 1.0f, MR_OnworldEnvironmentConstants.groundLerpFactor.x );
	}

	OW_Fog_ApplyExtinction( aEmission, fog );
	aLightOut.rgb += aEmission;
}