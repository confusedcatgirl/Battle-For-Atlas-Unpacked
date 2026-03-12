import "shaders/modules/graphics/onward/gi/fog/fog_light.h"

float3 OW_SpaceFog_GetCustomSunLight( in float3 aSunEmission, in float3 aPos, in float3 aViewDir, in float anAnisotropy )
{
	aSunEmission *= OW_FogConstantBuffer.myGalaxy.mySunIntensity; // fog scale factor

	return aSunEmission * OW_FogLight_GetSunLightPhase(
		MR_OnworldEnvironmentConstants.originalDirLightDir, aViewDir, anAnisotropy );
}

float3 OW_SpaceFog_GetSunLight( in float3 aPos, in float3 aViewDir, in float anAnisotropy, in float3 aShadowFactor )
{
	float3 sunEmission = MR_OnworldEnvironmentConstants.inSpaceSunLightColor * aShadowFactor;
	return OW_SpaceFog_GetCustomSunLight( sunEmission, aPos, aViewDir, anAnisotropy );
}
