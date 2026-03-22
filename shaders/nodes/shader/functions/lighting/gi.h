// ONWARD AJD INTEGRATE: this file had a lot of conflicts, took ours, EXCEPT this bit which was needed for like a billion shaders to compile
// really hackish way of avoiding redundant GI samplings without breaking legacy shaders,
// enabled in "complete" shaders where the call order is known
global bool ourAllowUseFirstSampleGI = false;
global bool ourUseFirstSampleGI = false;
global float3 ourFirstSampleGIResult = float3(0,0,0);
// end from integrate

import "shaders/modules/graphics/environment_constants.h"
import "shaders/modules/graphics/lighting_constants.h"
import "shaders/modules/graphics/lighting_data.h"

import "shaders/modules/graphics/onworld/onworld_environment.h"

import "shaders/modules/graphics/onward/common/samplers_gi.h"
import "shaders/modules/graphics/onward/gi/gi_sampling.h"

import "shaders/modules/graphics/onworld/gi_common.h"

@ifdef OGI_IS_BAKING
import "shaders/modules/graphics/onworld/gi_baking.h"
@endif // OGI_IS_BAKING

import "shaders/nodes/Shader/Functions/Onward/Lighting/OW_SpaceGI.h"

struct SProbeConstants
{
	float3 Position;
	float3 BoxMin;
	float3 BoxMax;
	uint IsIndoorOutdoor; //Actually two packed half floats.
};

global float ourCleanFactor = 1.0f;

float3 SampleGI( float3 normal, float3 worldPosition, float3 upVector, bool indoor, int roomIndex )
{
	float3 galaxyGI = OW_Space_GI( normal, worldPosition );

@ifdef ISNT_MAIN_PLANET
	return galaxyGI;
@else

	@ifndef OGI_IS_BAKING_PLANET

		float4 planetGI = MR_SampleLod( PlanetGndColorGICubemap, upVector, 0.0f );
		planetGI.xyz = lerp( planetGI.xyz, MR_SampleLod( PlanetSkyColorGICubemap, upVector, 0.0f ).xyz,
			OW_GI_GetHemisphericFactor( normal, upVector ) );

		planetGI.xyz = OW_GI_SamplePlanet_NOSTRENGTHFALLBACK( worldPosition, upVector, normal, planetGI.xyz );

		@ifndef LIGHT_FROM_GROUND
			planetGI = lerp( float4( galaxyGI, 1.0f ), planetGI, MR_OnworldEnvironmentConstants.groundLerpFactor.x );
		@endif // LIGHT_FROM_GROUND

		@ifndef LIGHT_FROM_SPACE
			ourCleanFactor = min(MR_OnworldEnvironmentConstants.oneMinusCreepFactor, planetGI.a);
		@endif // LIGHT_FROM_SPACE

		return planetGI.rgb;

	@else

		return OW_BakingGI_GetGI( worldPosition, normal, upVector );

	@endif // OGI_IS_BAKING_PLANET

@endif // ISNT_MAIN_PLANET
}

float3 OW_GI_Sample( float3 aDirection, float3 aWorldPosition, float3 anUpVector )
{
	return SampleGI( aDirection, aWorldPosition, anUpVector, false, -1 );
}

float3 OW_GI_Sample( in float3 aDirection, in OW_PositionContext aPosCtx )
{
	return OW_GI_Sample( aDirection, aPosCtx.mySphereAbsolutePos, aPosCtx.mySphereDir );
}

// probably wrong
float3 SampleGI( float3 normal, float3 worldPosition, bool indoor )
{
	return SampleGI( normal, worldPosition, normalize( worldPosition ), indoor, -1 );
}

float3 SampleGI( float3 normal, float3 worldPosition )
{
	return SampleGI( normal, worldPosition, false );
}

// probably wrong
float3 SampleGI( float3 normal, Gfx_LightingDataStruct lightingData : Gfx_LightingData )
{
    return SampleGI( normal, lightingData.worldPosition, normalize( lightingData.worldPosition ), lightingData.isIndoor, lightingData.roomIndex);
}
