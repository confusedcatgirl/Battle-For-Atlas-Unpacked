import "shaders/modules/graphics/onward/ow_environment_constants.h"

struct Gfx_DirectionalLight
{
	float3 toLightDirection;
	float3 emission;

	float sunHalfConeCos;
	float sunRotationToHalfConeCos;
	float sunRotationToHalfConeSin;
};

Gfx_DirectionalLight GetDeprecatedDirectionalLight( )
{
	Gfx_DirectionalLight light;

	light.toLightDirection = float3(0.0f, 1.0f, 0.0f);

	const float angle = 0.7f * 3.141592653589793238f / 180.0f;
	light.sunHalfConeCos = cos( angle );

	light.sunRotationToHalfConeCos = cos( angle * 0.5f );
	light.sunRotationToHalfConeSin = sin( angle * 0.5f );

	light.emission = float3(10.0f, 0.0f, 10.0f);

	return light;
}

constants Gfx_Environment : MR_EnvironmentConstants
{
	float3 shadowLightToLightDirection;
	float3 shadowLightEmission;
	bool shadowLightEmissionIsNonZero;
	float3 shadowLightAboveCloudsEmission;

	float3 ambient;
	float noiseFrame;

	float time;
	float realTime;
	float frameDeltaTime;
	uint frameIndex;

	float parallaxPOMMinSamples;
	float parallaxPOMMaxSamples;
	float parallaxScale;
	float parallaxDistance;
	float parallaxShadowStrength;
	float parallaxShadowScale;

	float screenSpaceReflectionQuality;
	int screenSpaceReflectionSupersampling;

	float4 GIDirections[6];
	float3 GITextureCoordScale;
	float GITextureFadeStart;
	float3 GITextureCoordBias;
	float GITextureFadeEnd;

	float averageLuminosity;

	float environmentMapFactor;
	float environmentMapLodFactor;
	float renderSunFactor;
	float defaultShadowFactor;

	float timeOfDay;
	float snowAmount;

	float interiorAmbient;
	float interiorLightIntensity;
	float interiorLitRoomFraction;
	bool useLightProbes;

	uint antialiasSamples;

	float3 FillLightGIShadowColor;
	float3 FillLightGISkyColor;
	float3 FillLightGIFrontColor;
	float3 FillLightGIBackColor;

	float4 weatherMaskTextureResolution;
	float4 weatherMaskUvTileCount;
	float2 weatherMaskTileResolution;
	float2 weatherMaskUvSamplingOffset;
	float4 weatherMaskWorldSize;

	float2 windVector;
	float2 accumulatedWind;
	float accumulatedWindDistance;

	float worldScale;

	float particleIntensityMultiplier;
	float emissiveIntensityMultiplier;

	bool isInUIPass;
}