//import "shaders/modules/graphics/onworld/functions/MiePhase.h"
import "shaders/modules/graphics/onworld/env_constants.h"

import "shaders/modules/graphics/onward/gi/volumetric_fog.h"
import "shaders/modules/graphics/onward/gi/volumetric_fog_constantbuffer.h"

import "shaders/modules/graphics/onward/common/samplers_gi.h"
@ifndef OGFX_LOW_QUALITY_FOG_SAMPLING
import "shaders/modules/graphics/shadows_common.h"
@endif // !OGFX_LOW_QUALITY_FOG_SAMPLING

import "shaders/modules/graphics/onward/gi/fog/space_fog_light_sun.h"

MR_Sampler3D SpaceVolFogTexture : OW_GalaxyVolFogTexture { filter = linear, wrap = clamp, anisotropy = 0 };
MR_Sampler2D SpaceFarFogTexture : OW_GalaxyFarFogTexture { filter = linear, wrap = clamp, anisotropy = 0 };
MR_Sampler3D SpaceLightVolume : OW_GalaxyLightVolume{ filter = linear, wrap = clamp, anisotropy = 0 };

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Standard functions (same as ground fog)
//---------------------------------------------------------------------------------------------------------------------------------------------------

OW_FogVolume3DCoords OW_SpaceFog_GetTransfromedCoords( in OW_VolumetricFog3DCoords aCoords )
{
	return OW_FogVolume_GetCoords( aCoords, OW_FogConstantBuffer.myGalaxy.myNear, OW_FogConstantBuffer.myGalaxy.myInvDistributionRange, OW_FogConstantBuffer.myGalaxy.myDistributionPower );
}

OW_FogVolume3DCoords OW_SpaceFog_GetCoordsFromPos( in float3 aWorldPos )
{
	return OW_SpaceFog_GetTransfromedCoords( OW_VolumetricFog_GetCoords( aWorldPos ) );
}

float4 OW_SpaceFog_GetFarFromLookup( in OW_FogVolume2DCoords aCoords )
{
	return MR_SampleLod0( SpaceFarFogTexture, aCoords.myUV );
}

float4 OW_SpaceFog_GetFarFromLookupHQ( in OW_FogVolume2DCoords aCoords, in float2 aPixelPosition )
{
	float4 fog = MR_SampleLod0( SpaceFarFogTexture, aCoords.myUV );
	@ifndef OGFX_LOW_QUALITY_FOG_SAMPLING
		aCoords.myUV += GetRotateVec( aPixelPosition ) * OW_SS_NOISE_OFFSET;
		fog = ( fog + MR_SampleLod0( SpaceFarFogTexture, aCoords.myUV ) ) * 0.5f;
	@endif // !OGFX_LOW_QUALITY_FOG_SAMPLING
	return fog;
}

float4 OW_SpaceFog_GetFromCoords( in OW_FogVolume3DCoords aCoords )
{
	return MR_SampleLod0( SpaceVolFogTexture, float3( aCoords.my2D.myUV, aCoords.myW ) );
}

float4 OW_SpaceFog_GetFromCoordsHQ( in OW_FogVolume3DCoords aCoords, in float2 aPixelPosition )
{
	float4 fog = MR_SampleLod0( SpaceVolFogTexture, float3( aCoords.my2D.myUV, aCoords.myW ) );
	@ifndef OGFX_LOW_QUALITY_FOG_SAMPLING
		aCoords.my2D.myUV += GetRotateVec( aPixelPosition ) * OW_SS_NOISE_OFFSET;
		fog = ( fog + MR_SampleLod0( SpaceVolFogTexture, float3( aCoords.my2D.myUV, aCoords.myW ) ) ) * 0.5f;
	@endif // !OGFX_LOW_QUALITY_FOG_SAMPLING
	return fog;
}

