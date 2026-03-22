import "shaders/modules/graphics/onworld/sun_direct.h"

import "shaders/modules/graphics/onworld/gi_common.h"

import "shaders/nodes/shader/functions/onward/OW_GetTerrainLowResHeight.h"
import "shaders/nodes/shader/functions/onward/OW_GetTerrainLowResAlbedo.h"

import "shaders/modules/graphics/onworld/functions/RaySphereIntersection.h"

import "shaders/modules/graphics/onward/gi/ground_fog.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float3 OW_GIBaking_GetDirectLighting( in float3 aPosition, in float3 anUpVector )
{
	float3 radiance = ( MR_OnworldEnvironmentConstants.onPlanetDirLightColor );
	radiance *= OW_Sun_GetAttenuation( MR_OnworldEnvironmentConstants.adjustedDirLightDir, anUpVector );

	const float3 terrainNormal = OW_GetTerrainLowResNormal( anUpVector );
	float diffuse = saturate( dot( MR_OnworldEnvironmentConstants.adjustedDirLightDir, terrainNormal ) );
	return radiance * diffuse / MR_PI;
}

float3 OW_GIBaking_GetFogColor( in float3 aPosition, in float3 aViewDir )
{
	return 0.0f;
}

float4 OW_GIBaking_GetFog( in float3 aPosition, in float3 aViewDir )
{
	return float4( OW_GIBaking_GetFogColor( aPosition, aViewDir ), 1.0f );
}

float3 OW_GIBaking_FogColor( in float3 aColor, in float4 aFog )
{
	return aColor.rgb * aFog.a + aFog.rgb;
}

float4 OW_GIBaking_GetSkyColorAndVisibility( in float3 aPosition, in float3 aDirection, in float3 anUpVector )
{
	float4 result = 0;
	result.a = saturate( dot( aDirection, anUpVector ) * 0.5f + 0.5f );

	anUpVector = MR_TransformNormal( MR_ViewConstants.worldToCamera, anUpVector );

	float4 color = float4( MR_SampleLod0( SkyColourLookup, anUpVector ).rgb, 1.0f );

	for (int i=0;i<16;++i)
	{
		float z = ( i / 15.0f );
		z = (z * 2.0f - 1.0f);

		for (int j=0;j<8;++j)
		{
			float a = ( j /  7.0f );

			float r = sqrt(1.0f - z * z);
			float p = a * MR_PI * 2.0;
			float x = r * cos(p);
			float y = r * sin(p);

			float3 eyeDir = MR_TransformNormal( MR_ViewConstants.worldToCamera, float3(x, y, z) );
			float w = dot( anUpVector, eyeDir );
			if (w > 0.0f)
			{
				color.rgb += MR_SampleLod0( SkyColourLookup, eyeDir ).rgb * w;
				color.a += w;
			}
		}
	}

	result.rgb = OW_GIBaking_FogColor( color.rgb / color.a, OW_GIBaking_GetFog( aPosition, aDirection ) );

	return result;
}

float3 OW_GIBaking_GetLitGroundColor( in float3 aPosition, in float3 aDirection, in float3 anUpVector, in float3 aIndirectLighting )
{
	float3 directLighting = OW_GIBaking_GetDirectLighting( aPosition, anUpVector );
	directLighting *= GetShadowFactor( aPosition, vec2(0) );
	float3 light = aIndirectLighting + directLighting;
	light *= OW_GetTerrainLowResAlbedo( anUpVector );

	return OW_GIBaking_FogColor( light, OW_GIBaking_GetFog( aPosition, aDirection ) );
}

float3 OW_BakingGI_GetGI( in float3 aPosition, in float3 aDirection, in float3 anUpVector )
{
	const float4 sky = OW_GIBaking_GetSkyColorAndVisibility( aPosition, aDirection, anUpVector );

	float3 groundColor = OW_GIBaking_GetLitGroundColor( aPosition, aDirection, anUpVector, sky.rgb );
	groundColor *= saturate( dot( aDirection, -anUpVector ) * 0.5 + 0.5 ) / (4.0 * MR_PI);

	float3 gi = ( sky.rgb * sky.a ) + groundColor.rgb;
	return lerp( vec3( 0 ), gi * OW_GIConstants.myPlanet.myStrength, OW_Planet_GetIsMainPlanet( ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////