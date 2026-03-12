import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/modules/graphics/onworld/atmosphere/bruneton/atmosphere.h"
import "shaders/modules/graphics/onworld/atmosphere/bruneton/common.h"
import "shaders/modules/graphics/onworld/atmosphere/planet_sky.h"

float3 OW_SkyModel_RemapPosition( in float3 aX, out float3 aD, out float aR)
{
	float atmosphereRadius = OW_Planet_GetAtmosphereRadius();
	float minTerrainRadius = OW_Planet_GetMinTerrainRadius();

	aR = clamp( length(aX), minTerrainRadius, atmosphereRadius );
	aR = (aR - minTerrainRadius) / (atmosphereRadius - minTerrainRadius);
	aR *= aR;
	aR = ( aR * 60.0f) + 6360.0f;

	aD = normalize(aX);
	return aD * aR;
}

float3 OW_SkyModel_GetToSunDirection( in float3 aX )
{
	return OW_PlanetConstants.const_PlanetToSunPosition.xyz;
}

float3 OW_SkyModel_GetAtmosphereColor(in float3 x, in float3 v, in float3 s)
{
	float4 xyzDir_wR;
	x = OW_SkyModel_RemapPosition( x, xyzDir_wR.xyz, xyzDir_wR.w );
	return Inscatter_Below( x, v, s, xyzDir_wR.w, dot( xyzDir_wR.xyz, v ) );
}

float3 OW_SkyModel_GetSunColor(in float3 x, in float3 v, in float3 s)
{
	float4 xyzDir_wR;
	x = OW_SkyModel_RemapPosition( x, xyzDir_wR.xyz, xyzDir_wR.w );
	return TransmittanceWithShadow( xyzDir_wR.w, dot(xyzDir_wR.xyz, v) );
}

float3 OW_SkyModel_GetSunColor(in float3 x, in float3 s)
{
	return OW_SkyModel_GetSunColor( x, s, s );
}
