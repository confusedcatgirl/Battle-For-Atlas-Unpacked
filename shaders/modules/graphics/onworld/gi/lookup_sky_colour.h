import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/onworld/gi/sky_sample.h"

import "shaders/modules/graphics/onworld/functions/RaySphereIntersection.h"
import "shaders/modules/graphics/onward/common/samplers_lookup.h"

float OW_SkyModel_GetCloudsShadows( in float3 aPos )
{
	const float3 sunPosition = OW_PlanetConstants.const_RelativeSunPosition.xyz;
	const float3 direction = normalize(aPos - sunPosition);
	return OW_Sky_GetCloudsShadowsFromInside( aPos, direction );
}

float OW_SkyModel_GetCloudsShadows( in float3 anEyePos, in float3 aEyeDir )
{
	float2 inter = OW_RaySphereIntersection( anEyePos, aEyeDir, OW_Planet_GetAtmosphereRadius( ) );
	return OW_SkyModel_GetCloudsShadows( anEyePos + aEyeDir * inter.y );
}

bool OW_SkyModel_GetSkyColorFromInside( out float4 aColor, in float3 anEyePos, in float3 aEyeDir, in bool aSelfOcclusion )
{
	const float3 sunDir = OW_SkyModel_GetToSunDirection( anEyePos );

	const float radius = OW_Planet_GetMinTerrainRadius( );
	float remappingRadius = 6360.0f;

	// remap the eye position
	float4 remappedDirLength;
	float3 remappedEyePos = OW_SkyModel_RemapPosition( anEyePos, remappedDirLength.xyz, remappedDirLength.w );

	const float2 inter = OW_RaySphereIntersection( remappedEyePos, aEyeDir, remappingRadius );

	if (!aSelfOcclusion || (inter.x >= inter.y || inter.x < 0.0f)) // can we actually see the sky?
	{
		aColor = float4( 0.0f, 0.0f, 0.0f, 0.0f );//OW_Sky_GetCloudsColorFromBelow( anEyePos, aEyeDir );

		float4 atmosphereColor = float4( 0.0f, 0.0f, 0.0f, 1.0f );

		atmosphereColor.rgb = OW_SkyModel_GetAtmosphereColor( anEyePos, aEyeDir, sunDir );
		atmosphereColor.rgb = max( atmosphereColor.rgb, MR_OnworldEnvironmentConstants.onPlanetNightSkyColor.rgb );

		aColor = OW_Sky_BlendAtmosphereAndClouds( atmosphereColor, aColor );

		return true;
	}
	else
	{
		aColor = vec4( 0.0f );
	}
	return false;
}
