import "shaders/modules/graphics/onworld/sun_color.h"
import "shaders/modules/graphics/onworld/sun_light.h"

import "shaders/nodes/Shader/Functions/Onward/Lighting/OW_GetMixedFog.h"

import "shaders/modules/graphics/onworld/planet_flattening.h"

float OW_GetParticleMultiplier( in float3 anUpVector, in float3 aGI, in float3 aToSunDir )
{
	// return dot(aGI, float3(0.3f, 0.59f, 0.11f));

	float sunDotPos = dot( aToSunDir, anUpVector );
	sunDotPos = sqrt( sqrt( saturate( sunDotPos ) ) );

	// the following returns 0.5 at night, 2.5 under the sun and 1.0 in space
	return 0.5f + lerp( 0.5f, sunDotPos * 2.0f, MR_OnworldEnvironmentConstants.groundLerpFactor.x );
}

void OW_SampleFogForParticles( in float3 aFlattenedPosition, in float2 aPixel, out float4 aFog )
{
	aFog = OW_GetMixedFog( aFlattenedPosition, aPixel );
}

void OW_SampleGIForParticles( in float3 aFlattenedPosition, in float3 anUpVector, out float3 aGI )
{
	float3 direction = - MR_ViewConstants.nearPlane.xyz;
	aGI = OW_GI_Sample( direction, aFlattenedPosition, anUpVector );
}

void OW_SampleParticles( in float3 anUnflattenedPosition, in float3 aFlattenedPosition, in float3 anUpVector, out float3 anAmbient, out float3 aToSunDir, out float3 aSunColor, out float4 aFog )
{
	OW_SampleGIForParticles( aFlattenedPosition, anUpVector, anAmbient );

	float3 temp1 = 0.0f;
	float3 temp2 = 0.0f;
	OW_Sun_GetShadowLightDirAndColor( anUnflattenedPosition,
	    anUpVector, anUpVector, temp1, aToSunDir, aSunColor, temp2 );
	aSunColor = lerp( MR_OnworldEnvironmentConstants.inSpaceSunLightColor, aSunColor, MR_OnworldEnvironmentConstants.groundLerpFactor.x );
	float2 clipPos = MR_Transform( MR_ViewConstants.worldToClip, aFlattenedPosition).xy;
	float2 pixel = MR_ClipToPixel( clipPos );
	OW_SampleFogForParticles( aFlattenedPosition, pixel, aFog );
}

void OW_SampleParticles( in float3 anUnflattenedPosition, in float3 aFlattenedPosition, in float3 anUpVector, in float2 aPixel, out float3 anAmbient, out float3 aToSunDir, out float3 aSunColor, out float4 aFog )
{
	OW_SampleGIForParticles( aFlattenedPosition, anUpVector, anAmbient );

	float3 temp1 = 0.0f;
	float3 temp2 = 0.0f;
	OW_Sun_GetShadowLightDirAndColor( anUnflattenedPosition,
	    anUpVector, anUpVector, temp1, aToSunDir, aSunColor, temp2 );
	aSunColor = lerp( MR_OnworldEnvironmentConstants.inSpaceSunLightColor, aSunColor, MR_OnworldEnvironmentConstants.groundLerpFactor.x );
	OW_SampleFogForParticles( aFlattenedPosition, aPixel, aFog );
}

void OW_SampleParticles( in float3 anUnflattenedPosition, in float3 anUpVector, out float3 anAmbient, out float3 aToSunDir, out float3 aSunColor, out float4 aFog )
{
	float3 flattenedPosition = anUnflattenedPosition;
	OW_SFT_ApplyTmp( flattenedPosition ); // required to continue

	OW_SampleParticles( anUnflattenedPosition, flattenedPosition, anUpVector, anAmbient, aToSunDir, aSunColor, aFog );
}