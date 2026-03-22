import "shaders/nodes/shader/functions/onward/OW_GetSunBodyIntensityMultiplier.h"

float3 GetSunLightColorInPlanet()
{
	return MR_OnworldEnvironmentConstants.inPlanetSunLightColor;
}

float3 GetSunLightColorInSpace()
{
	return MR_OnworldEnvironmentConstants.inSpaceSunLightColor;
}

float3 OW_GetSunBodyColor( )
{
	return lerp(GetSunLightColorInPlanet( ), GetSunLightColorInSpace( ), OW_GetSunBodyIntensityMultiplier( )) * vec3(10);
}
