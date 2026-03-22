import "shaders/MRender/MR_ViewConstants.h"

const float OW_SS_NOISE_OFFSET = ( 0.5f / 32.0f );

// This is basically to avoid using the wrong functions.  Hopefully, this is optimized properly.

struct OW_FogVolume2DCoords
{
	float2 myUV;
};

struct OW_FogVolume3DCoords
{
	OW_FogVolume2DCoords my2D;
	float myW;
};

struct OW_VolumetricFog3DCoords
{
	OW_FogVolume2DCoords my2D;
	float myDepth;
};

//-----------------------------------
// 2D
OW_FogVolume2DCoords OW_FogVolume_GetCoordsUV( in float2 aScreenUV )
{
	OW_FogVolume2DCoords coords;
	coords.myUV = aScreenUV;
	return coords;
}

float2 OW_FogVolume_CPToUV( in float2 aClipPos )
{
	return aClipPos * float2(0.5, -0.5) + vec2(0.5);
}

OW_FogVolume2DCoords OW_FogVolume_GetCoordsCP( in float2 aClipPos )
{
	return OW_FogVolume_GetCoordsUV( OW_FogVolume_CPToUV( aClipPos ) );
}

OW_FogVolume2DCoords OW_FogVolume_GetCoordsFC( in float4 aFragCoord )
{
	return OW_FogVolume_GetCoordsCP( MR_PixelToClip( aFragCoord.xy ) );
}

OW_FogVolume2DCoords OW_FogVolume_GetCoordsPos( in float3 aWorldPos )
{
	const float4 clipPos = MR_Transform( MR_ViewConstants.worldToClip, aWorldPos );
	return OW_FogVolume_GetCoordsCP( clipPos.xy / clipPos.w );
}

OW_FogVolume3DCoords OW_FogVolume_GetCoords( in OW_VolumetricFog3DCoords aCoords, in float aDistributionNear, in float aDistributionRangeInv, in float aDistributionPower )
{
	OW_FogVolume3DCoords coords;
	coords.my2D = aCoords.my2D;
	coords.myW = pow(saturate((aCoords.myDepth - aDistributionNear) * aDistributionRangeInv), aDistributionPower);
	return coords;
}

//-----------------------------------
// 3D... distance based
OW_VolumetricFog3DCoords OW_VolumetricFog_GetCoordsUV( in float2 aScreenUV, in float aDepth )
{
	OW_VolumetricFog3DCoords coords;
	coords.my2D = OW_FogVolume_GetCoordsUV( aScreenUV );
	coords.myDepth = aDepth;
	return coords;
}

OW_VolumetricFog3DCoords OW_VolumetricFog_GetCoordsCP( in float2 aClipPos, in float aDepth )
{
	OW_VolumetricFog3DCoords coords;
	coords.my2D = OW_FogVolume_GetCoordsCP( aClipPos );
	coords.myDepth = aDepth;
	return coords;
}

OW_VolumetricFog3DCoords OW_VolumetricFog_GetCoordsFC( in float4 aFragCoord, in float aDepth )
{
	OW_VolumetricFog3DCoords coords;
	coords.my2D = OW_FogVolume_GetCoordsFC( aFragCoord );
	coords.myDepth = aDepth;
	return coords;
}

//-----------------------------------
//

OW_VolumetricFog3DCoords OW_VolumetricFog_GetCoords( in float3 aWorldPos )
{
	OW_VolumetricFog3DCoords coords;
	coords.my2D = OW_FogVolume_GetCoordsPos( aWorldPos );
	coords.myDepth = length( aWorldPos - MR_GetPos( MR_ViewConstants.cameraToWorld ).xyz );
	return coords;
}

//-----------------------------------
//

void OW_Fog_ApplyExtinction(inout float3 aColor, in float aExtinction)
{
	aColor = aColor * aExtinction;
}

void OW_Fog_ApplyExtinction(inout float3 aColor, in float4 aFog)
{
	OW_Fog_ApplyExtinction( aColor, aFog.a );
}

void OW_Fog_AddFogColor(inout float3 aColor, in float3 aFog)
{
	aColor += aFog;
}

void OW_Fog_AddFogColor(inout float3 aColor, in float4 aFog)
{
	OW_Fog_AddFogColor( aColor, aFog.rgb );
}

void OW_Fog_Apply(inout float3 aColor, in float4 aFog)
{
	aColor = aColor * aFog.a + aFog.rgb;
}

//-----------------------------------

float OW_FogLight_GetPhaseFunction(in float aCos, in float aG)
{
	return (1.0 - aG*aG) / (4.0 * MR_PI * pow( 1.0 + aG*aG - 2.0*aG*aCos, 1.5 ) );
}

//-----------------------------------
