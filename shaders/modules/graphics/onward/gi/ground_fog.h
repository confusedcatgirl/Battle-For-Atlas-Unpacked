import "shaders/modules/graphics/onworld/env_constants.h"

import "shaders/modules/graphics/onward/gi/volumetric_fog.h"
import "shaders/modules/graphics/onward/gi/volumetric_fog_constantbuffer.h"

@ifndef OGFX_LOW_QUALITY_FOG_SAMPLING
import "shaders/modules/graphics/shadows_common.h"
@endif // !OGFX_LOW_QUALITY_FOG_SAMPLING

MR_Sampler3D GroundVolFogTexture : OW_PlanetVolFogTexture { filter = linear, wrap = clamp, anisotropy = 0 };
MR_Sampler2D GroundFarFogTexture : OW_PlanetFarFogTexture { filter = linear, wrap = clamp, anisotropy = 0 };
MR_Sampler3D PlanetLightVolume : OW_PlanetLightVolume{ filter = linear, wrap = clamp, anisotropy = 0 };

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Standard functions
//---------------------------------------------------------------------------------------------------------------------------------------------------

float OW_GroundFog_GetFarExtinction( ) { return OW_FogConstantBuffer.myPlanet.myFarExtinction; }
float OW_GroundFog_GetFarScattering( ) { return OW_FogConstantBuffer.myPlanet.myFarScattering; }

OW_FogVolume3DCoords OW_GroundFog_GetTransfromedCoords( in OW_VolumetricFog3DCoords aCoords )
{
	return OW_FogVolume_GetCoords( aCoords, OW_FogConstantBuffer.myPlanet.myNear, OW_FogConstantBuffer.myPlanet.myInvDistributionRange, OW_FogConstantBuffer.myPlanet.myDistributionPower );
}

OW_FogVolume3DCoords OW_GroundFog_GetCoordsFromPos( in float3 aWorldPos )
{
	return OW_GroundFog_GetTransfromedCoords( OW_VolumetricFog_GetCoords( aWorldPos ) );
}

float4 OW_GroundFog_GetFarFromLookup( in OW_FogVolume2DCoords aCoords )
{
	return MR_SampleLod0( GroundFarFogTexture, aCoords.myUV );
}

float4 OW_GroundFog_GetFarFromLookupHQ( in OW_FogVolume2DCoords aCoords, in float2 aPixelPosition )
{
	float4 fog = MR_SampleLod0( GroundFarFogTexture, aCoords.myUV );

	@ifndef OGFX_LOW_QUALITY_FOG_SAMPLING
		aCoords.myUV += GetRotateVec( aPixelPosition ) * OW_SS_NOISE_OFFSET;
		fog = ( fog + MR_SampleLod0( GroundFarFogTexture, aCoords.myUV ) ) * 0.5f;
	@endif // !OGFX_LOW_QUALITY_FOG_SAMPLING

	return fog;
}

float4 OW_GroundFog_GetFromCoords( in OW_FogVolume3DCoords aCoords )
{
	return MR_SampleLod0( GroundVolFogTexture, float3( aCoords.my2D.myUV, aCoords.myW ) );
}

float4 OW_GroundFog_GetFromCoordsHQ( in OW_FogVolume3DCoords aCoords, in float2 aPixelPosition )
{
	float4 fog = MR_SampleLod0( GroundVolFogTexture, float3( aCoords.my2D.myUV, aCoords.myW ) );
	@ifndef OGFX_LOW_QUALITY_FOG_SAMPLING
		aCoords.my2D.myUV += GetRotateVec( aPixelPosition ) * OW_SS_NOISE_OFFSET;
		fog = ( fog + MR_SampleLod0( GroundVolFogTexture, float3( aCoords.my2D.myUV, aCoords.myW ) ) ) * 0.5f;
	@endif // !OGFX_LOW_QUALITY_FOG_SAMPLING
	return fog;
}

float OW_GroundFog_GetCameraBasedSkyBoost( in float aHeight )
{
	return aHeight * 0.001f;
}

float OW_GroundFog_GetSkyBoost( in float aCameraBasedSkyBoost, in float aHeight )
{
	return saturate( aCameraBasedSkyBoost + aHeight * 0.001f );
}
