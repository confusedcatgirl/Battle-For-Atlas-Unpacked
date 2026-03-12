import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/modules/graphics/lights_common.h"
import "shaders/modules/graphics/shadows_common.h"
import "shaders/modules/graphics/spot_shadows.h"
import "shaders/modules/graphics/shadows.h"
import "shaders/modules/graphics/onworld/sun_color.h"
import "shaders/modules/graphics/onworld/sun_light.h"
import "shaders/modules/graphics/onworld/onworld_gi.h"

import "shaders/modules/graphics/onward/lighting/light.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------
// .##......######...####...##..##..######...####..
// .##........##....##......##..##....##....##.....
// .##........##....##.###..######....##.....####..
// .##........##....##..##..##..##....##........##.
// .######..######...####...##..##....##.....####..
//---------------------------------------------------------------------------------------------------------------------------------------------------

@ifdef OGFX_STATIC_SHADOW
	@define OGFX_STATIC_CLOUDS_SHADOW
@endif // OGFX_STATIC_SHADOW

@ifdef OGFX_STATIC_CLOUDS_SHADOW
	@define OGFX_ATTENUATE_SHADOWS
@endif // OGFX_STATIC_CLOUDS_SHADOW

struct OW_SunLight
{
	float3 myToSunDir;
	float3 myRadiance;
};

struct OW_SunLightContext
{
	OW_SunLight mySunLight;
	float3 myPixelToSunDir;

@ifdef LIGHT_FROM_SPACE
	float3 mySunToSkyIntersection;
@endif // LIGHT_FROM_SPACE

@ifdef LIGHTING_TRANSITION
	float3 mySunToSkyIntersection;
	float3 mySunToSkyIntersectionCloudsShadows;
@endif // LIGHTING_TRANSITION

};

struct OW_SunLitPixel
{
	OW_PositionContext myPosCtx;
	OW_SunLightContext mySunCtx;
};

float OW_Lighting_GetCloudsShadowFactor(in OW_SunLitPixel aCtx, in Gfx_LightingDataStruct aLightingData)
{
@ifdef CLOUDS_ENABLED

	@ifdef LIGHT_FROM_SPACE
		float fromSpace = OW_Sky_GetCloudsShadowsFromSpace(normalize(aCtx.mySunCtx.mySunToSkyIntersection));
	@endif // LIGHT_FROM_SPACE
	@ifdef LIGHTING_TRANSITION
		float fromSpace = lerp(1.0f, OW_Sky_GetCloudsShadowsFromSpace(normalize(aCtx.mySunCtx.mySunToSkyIntersectionCloudsShadows)), OW_PlanetConstants.const_CloudsShadowsFade);
	@endif // LIGHTING_TRANSITION
	@ifdef IS_SIMPLIFIED

		@ifdef LIGHT_FROM_SPACE
			return lerp(1.0, fromSpace, OW_PlanetConstants.const_CloudsEnabled);
		@endif // LIGHT_FROM_SPACE

	@else

		float3 shadowDir = aCtx.mySunCtx.myPixelToSunDir;

		@ifdef OGFX_STATIC_CLOUDS_SHADOW
			shadowDir = MR_OnworldEnvironmentConstants.originalDirLightDir;
		@endif // OGFX_STATIC_CLOUDS_SHADOW

		return aCtx.myPosCtx.myIsPosInSpace ? 1.0 :
			@ifdef LIGHT_FROM_SPACE
				fromSpace
			@else
				@ifdef LIGHTING_TRANSITION
					lerp(
					OW_Sky_GetCloudsShadowsFromInside(aCtx.myPosCtx.mySphereRelativePos, shadowDir, aLightingData.depth),
						fromSpace, OW_PlanetConstants.const_CloudsShadowsFade)
				@else
					OW_Sky_GetCloudsShadowsFromInside(aCtx.myPosCtx.mySphereRelativePos, shadowDir, aLightingData.depth)
				@endif // LIGHTING_TRANSITION
			@endif // LIGHT_FROM_SPACE
			;

	@endif // IS_SIMPLIFIED

@else
	return 1.0;
@endif // CLOUDS_ENABLED
}

float3 OW_Lighting_GetSunShadow(in OW_SunLitPixel aCtx, in Gfx_LightingDataStruct aLightingData, in float4 csmAttenuation)
{
	float4 shadowFactor;

	shadowFactor.rgb = GetShadowFactor(aLightingData.worldPosition, aLightingData.pixelPosition);
@ifndef OGI_IS_BAKING
	shadowFactor.a = OW_Lighting_GetCloudsShadowFactor(aCtx, aLightingData);
@else
	shadowFactor.a = 1;
@endif// OGI_IS_BAKING

	shadowFactor = csmAttenuation * shadowFactor + (vec4(1.0) - csmAttenuation);
	return shadowFactor.rgb * shadowFactor.a;
}

void OW_Lighting_InitSunLitPixel(out OW_SunLightContext aLitPixel, in OW_PositionContext aPosCtx, in float3 aNormal)
{
	const float distanceToCam = length( MR_ViewConstants.LODCameraPosition - aPosCtx.myAbsolutePos );

	float3 pixelToSunDir2;
	float3 ignored;
	OW_Sun_GetShadowLightDirAndColor( aPosCtx.mySphereAbsolutePos, aPosCtx.mySphereRelativePos, aNormal, aPosCtx.mySphereDir, distanceToCam,
		aLitPixel.myPixelToSunDir, aLitPixel.mySunLight.myToSunDir, aLitPixel.mySunLight.myRadiance, ignored, pixelToSunDir2, aPosCtx.myAbsolutePos);
	aLitPixel.mySunLight.myRadiance = lerp( aLitPixel.mySunLight.myRadiance, MR_OnworldEnvironmentConstants.inSpaceSunLightColor, aPosCtx.mySpaceFactor );

@ifdef LIGHT_FROM_SPACE
	{
		const float3 sunToPixelDir = -aLitPixel.myPixelToSunDir;

		const float3 sunPosition = OW_PlanetConstants.const_RelativeSunPosition.xyz;
		float2 intersection = Sphere_RayIntersect(sunPosition, sunToPixelDir, OW_Planet_GetAtmosphereRadius());
		aLitPixel.mySunToSkyIntersection = sunToPixelDir * intersection.x + sunPosition;
	}
@endif // LIGHT_FROM_SPACE
@ifdef LIGHTING_TRANSITION
	const float3 sunPosition = OW_PlanetConstants.const_RelativeSunPosition.xyz;
	{
		const float3 sunToPixelDir = -aLitPixel.myPixelToSunDir;
		float2 intersection = Sphere_RayIntersect(sunPosition, sunToPixelDir, OW_Planet_GetAtmosphereRadius());
		aLitPixel.mySunToSkyIntersection = sunToPixelDir * intersection.x + sunPosition;
	}
	{
		const float3 sunToPixelDir = -pixelToSunDir2;
		float2 intersection = Sphere_RayIntersect(sunPosition, sunToPixelDir, OW_Planet_GetAtmosphereRadius());
		aLitPixel.mySunToSkyIntersectionCloudsShadows = sunToPixelDir * intersection.x + sunPosition;
	}
@endif // LIGHTING_TRANSITION
}

bool OW_Lighting_SunLight(inout OW_AttenuatedLight aPixelLight, in OW_SunLitPixel aCtx, in Gfx_LightingDataStruct aLightingData)
{
	float3 sunRadiance = aCtx.mySunCtx.mySunLight.myRadiance;
	if (dot(sunRadiance, 1) > 0)
	{
		float4 shadowAttenuation = vec4(1.0);

		@ifdef OGFX_ATTENUATE_SHADOWS

		@ifndef LIGHT_FROM_SPACE
			@ifndef ISNT_MAIN_PLANET

				@ifdef OGFX_STATIC_SHADOW
					float2 factor = float2( abs( dot( aCtx.myPosCtx.mySphereDir, MR_OnworldEnvironmentConstants.adjustedDirLightDir ) ),
					                   saturate( dot( aCtx.myPosCtx.mySphereDir, MR_OnworldEnvironmentConstants.originalDirLightDir ) ) );
				@else
					@ifdef OGFX_STATIC_CLOUDS_SHADOW
						float2 factor = float2( 1.0, saturate( dot( aCtx.myPosCtx.mySphereDir, MR_OnworldEnvironmentConstants.originalDirLightDir ) ) );
					@endif // OGFX_STATIC_CLOUDS_SHADOW
				@endif // OGFX_STATIC_SHADOW

				const float a = 0.422618261740699436186978489;
				const float b = 0.382683432365089771728459984;

				const float4 range = float4(a, b, (3.0 / a), (3.0 / b));
				shadowAttenuation = saturate( factor * range.zw - range.xy * range.zw ).xxxy;

			@endif // !ISNT_MAIN_PLANET
		@endif // !LIGHT_FROM_SPACE

		@endif // OGFX_ATTENUATE_SHADOWS

		sunRadiance *= OW_Lighting_GetSunShadow(aCtx, aLightingData, shadowAttenuation);
		if (dot(sunRadiance, 1) > 0)
		{
			aPixelLight.diffRadiance = sunRadiance;
			aPixelLight.specRadiance = sunRadiance;

			aPixelLight.diffToLight = aCtx.mySunCtx.mySunLight.myToSunDir;
			aPixelLight.specToLight = aCtx.mySunCtx.mySunLight.myToSunDir;

			return true;
		}
	}
	return false;
}
