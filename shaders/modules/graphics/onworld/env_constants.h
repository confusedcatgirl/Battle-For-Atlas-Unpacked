
constants MR_OnworldEnvironmentConstants : OW_EnvironmentConstants
{
	float dayToNightFactor;

	float inCreepFactor;
	float oneMinusCreepFactor;

	float free2;

	float2 groundLerpFactor;
	float2 groundFlatLength;

	float  spaceIntensity;
	float  dirLightConeCosAngle;
	float  dirLightConeCosHalfAngle;
	float  dirLightConeSinHalfAngle;

	float3 inSpaceSunLightColor;
	float  inSpaceSunLightIntensity;

	float3 onPlanetDirLightColor;       // Night light + Sun light combined into a single directional light with tricks

	float3 onPlanetSunLightColor;       // No tricks here
	float  onPlanetSunLightIntensity;

	float3 onPlanetNightLightColor;     // No tricks here
	float  onPlanetNightLightIntensity;

	float3 onPlanetNightAmbientColor;
	float  onPlanetNightAmbientIntensity;

	float3 onPlanetNightSkyColor;
	float3 onPlanetNightLightColorNoFactor;

	float3 inPlanetSunLightColor;

	// "directional light" direction
	float3 originalDirLightDir;
	float  free4;
	float3 originalDirLightPos;
	float  free5;

	float3 adjustedDirLightDir;
	float  pixelToSunDirParam1;

	float3 adjustedDirLightPos;
	float  pixelToSunDirParam2;
}