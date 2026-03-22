pixel
{
	import "shaders/MRender/MR_ViewConstants.h"

	import "shaders/nodes/Shader/Functions/Lighting/Environment Map.h"

	import "shaders/modules/graphics/lights_common.h"
	import "shaders/modules/graphics/onward/lighting/lighting_opacity.h"
	import "shaders/modules/graphics/onward/lighting/light.h"
	import "shaders/modules/graphics/onward/lighting/lighting_generic.h"
}

import "shaders/nodes/Shader/Functions/Onward/Lighting/OW_FullSpaceBaseLighting.h"

MR_SamplerCube OW_UIShipEnvironmentMap : OW_UIShipEnvironmentMap { anisotropy = 0 };

constants OW_UIShipLightingConstants : OW_UIShipLightingConstants
{
	float4 ambient[6];

	float environmentMapIntensity;
	float environmentMapLodFactor;

	float lerpFactor;

	uint  lightCount;
	uint4 lightData[40];
}

Gfx_Light OW_UIShip_GetLightFromConstantBuffer( uint aLightIndex )
{
	uint dataIndex = aLightIndex * 5;

	uint4 data[5];

	data[0] = OW_UIShipLightingConstants.lightData[dataIndex + 0];
	data[1] = OW_UIShipLightingConstants.lightData[dataIndex + 1];
	data[2] = OW_UIShipLightingConstants.lightData[dataIndex + 2];
	data[3] = OW_UIShipLightingConstants.lightData[dataIndex + 3];
	data[4] = OW_UIShipLightingConstants.lightData[dataIndex + 4];

	return Gfx_UnpackLight(data);
}

void OW_FullUIShipLighting(

	out float3 aLightOut,

	float3 aAlbedo,
	float3 aNormal,

	float aOpacity <default = 1>,

	float aRoughness <default = 1>,
	float aMetalness <default = 0>,

	float aOcclusion <default = 1>,

	float3 aEmission,

	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData
)
{
	const float3 worldPosition = MR_Transform(MR_ViewConstants.worldToCamera, float4(Gfx_LightingData.worldPosition, 1.0)).xyz;

	const float3 V = normalize(-worldPosition);

	const OGfx_Material material = OW_Material_Init(aRoughness, aMetalness, aOpacity);

	@include "shaders/modules/graphics/onward/lighting/lighting_code.h"
	OW_Lighting_Prepare(material, aNormal, V)

	// ----
	ambientDiff = OW_GI_SampleFallback(OW_UIShipLightingConstants.ambient, aNormal);
	// ----

	// ----
	const float3 envMap = OW_EnvMap_GetNormalizedSample_2(OW_UIShipEnvironmentMap,
		dominantSpecDir, material.roughness, ambientDiff, OW_UIShipLightingConstants.environmentMapLodFactor) * OW_UIShipLightingConstants.environmentMapIntensity;
	ambientSpec = OW_EnvMap_GetNormalizedSample_1(dominantSpecDir, material.roughness, ambientDiff);
	ambientSpec = lerp(ambientSpec, envMap, OW_UIShipLightingConstants.lerpFactor);
	// ----

	// ----
	#define DIRECT_SHIP_LIGHTING(X, Y)                                                                                                                           \
		for (uint i = 0; i < OW_UIShipLightingConstants.lightCount; ++i)                                                                                         \
		{                                                                                                                                                        \
			Gfx_Light light = OW_UIShip_GetLightFromConstantBuffer(i);                                                                                           \
			if (OW_Light_AttenuateLight(attenuatedLight, light, worldPosition, Gfx_LightingData.pixelPosition))                                                  \
			{                                                                                                                                                    \
				directDiff += attenuatedLight.diffRadiance * X(material, diffReflectance, normalize(attenuatedLight.diffToLight), aNormal);                      \
				directSpec += attenuatedLight.specRadiance * Y(material, specReflectance, normalize(attenuatedLight.specToLight), aNormal, V, gV, alphaSquared); \
			}                                                                                                                                                    \
		}

	OW_Lighting(DIRECT_SHIP_LIGHTING, aAlbedo, material)
	// ----

	aLightOut = ( ambientDiff + ambientSpec + directDiff + directSpec) * aOcclusion + aEmission; // no shadows, always use AO
}
