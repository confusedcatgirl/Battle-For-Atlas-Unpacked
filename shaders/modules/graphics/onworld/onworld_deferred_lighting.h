
import "shaders/modules/graphics/onward/lighting/brdf.h"
import "shaders/modules/graphics/onward/lighting/lighting_generic.h"
import "shaders/modules/graphics/onward/lighting/lighting_harvested.h"
import "shaders/modules/graphics/onward/lighting/lighting_opacity.h"
import "shaders/modules/graphics/onward/lighting/light.h"

import "shaders/modules/graphics/onward/creep/creep_constants.h"
import "shaders/modules/graphics/onward/creep/creep_funcs.h"

float3 OW_DeferredLighting(

	// inputs

	in float4 aDebugFlags,

	in OW_SunLitPixel aSunLitPixel,

	// material
	in float3 aAlbedo,
	in float3 aNormal,

	in float aRoughness,
	in float aMetalness,
	in float aOcclusion,

	in float aOpacity,
	in float aUnnamed,

	in Gfx_LightingDataStruct aLightingData,

	// outputs
	out float3 aDiffOut,
	out float3 aReflOut,
	out float3 aNormOut,
)
{
@ifdef OGI_DEBUG_INTENSITY
	aAlbedo = MR_GRAY_ALBEDO;
@endif // OGI_DEBUG_INTENSITY

	OGfx_Material material = OW_Material_Init(aRoughness, aMetalness, aOpacity);

	const float3 V = normalize(aLightingData.toCamera);

	if (aLightingData.isUnflatten)
	{
		aNormOut = aNormal;
	}
	else
	{
		// compute normal rotation
		float3 up = aSunLitPixel.myPosCtx.mySphereDir;
		float4 rot = float4(cross(up, aNormal), dot(up, aNormal) + 1.0);
		rot *= rsqrt(dot(rot, rot));

		// compute flattened normal
		up = aSunLitPixel.myPosCtx.myGroundDir;
		aNormOut = cross(rot.xyz, up) * 2.0;
		aNormOut = normalize(up + rot.w * aNormOut + cross(rot.xyz, aNormOut));
	}

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Combine ambient occlusion (baked & SSAO)
	@ifdef NO_SSAO_COMBINE
		float combinedAO = aOcclusion;
	@else
		@ifdef OGFX_DEFERRED
			float combinedAO = min(aOcclusion, aLightingData.SSAO);
		@else
			float combinedAO = aOcclusion;
		@endif // GFX_FORWARD_SHADER
	@endif // NO_SSAO_COMBINE
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	@include "shaders/modules/graphics/onward/lighting/lighting_code.h"
	OW_Lighting_Prepare(material, aNormOut, V)

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	ambientDiff = OW_GI_Sample(aNormal, aSunLitPixel.myPosCtx);

	// clean this up!
	@ifdef OGFX_DEFERRED
		@ifdef LIGHT_FROM_SPACE
			ourCleanFactor = aLightingData.isGround ? 0.0 : 1.0;
		@endif // LIGHT_FROM_SPACE
	@else
		@ifdef OGFX_IS_PLANET
			@ifdef LIGHT_FROM_SPACE
				ourCleanFactor = 0.0;
			@endif // LIGHT_FROM_SPACE
		@endif // OGFX_IS_PLANET
	@endif // OGFX_DEFERRED

	@ifdef DO_AMBIENT_OPACITY // used in forward only
		ambientDiff = lerp(OW_GI_Sample(-aNormal, aSunLitPixel.myPosCtx), ambientDiff, material.opacity);
	@endif // DO_AMBIENT_OPACITY

	@ifdef OGI_IS_BAKING
		ambientSpec = OW_GI_Sample(dominantSpecDir, aSunLitPixel.myPosCtx);
	@endif // OGI_IS_BAKING

	
	#define DIRECT_SUNLIGHT(X, Y)                                                                                                                                            \
		if (OW_Lighting_SunLight(attenuatedLight, aSunLitPixel, aLightingData))                                                                                              \
		{                                                                                                                                                                    \
			OW_Light_SunDisk(attenuatedLight, dominantSpecDir, MR_OnworldEnvironmentConstants.dirLightConeSinHalfAngle, alpha);                                              \
			                                                                                                                                                                 \
			directDiff += attenuatedLight.diffRadiance * X(material, diffReflectance, attenuatedLight.diffToLight, aNormal);                                                 \
			directSpec += attenuatedLight.specRadiance * Y(material, specReflectance, attenuatedLight.specToLight, aNormOut, V, gV, alphaSquared);                           \
		}

	@ifndef OGFX_NO_LIGHTS
		#define DIRECT_LIGHTING(X, Y)                                                                                                                                        \
			uint baseIndex;                                                                                                                                                  \
			uint numLights;                                                                                                                                                  \
			float2 minMax;                                                                                                                                                   \
			Gfx_GetTileData(baseIndex, numLights, minMax, aLightingData.pixelPosition);                                                                                      \
			if (aLightingData.depth >= minMax.x && aLightingData.depth <= minMax.y)                                                                                          \
			{                                                                                                                                                                \
				for (uint idx = 0; idx<numLights; ++idx)                                                                                                                     \
				{                                                                                                                                                            \
					Gfx_Light tileLight = Gfx_GetLightTile(baseIndex + idx);                                                                                                 \
					if (OW_Light_AttenuateLight(attenuatedLight, tileLight, aSunLitPixel.myPosCtx.myAbsolutePos, aLightingData.pixelPosition))                         \
					{                                                                                                                                                        \
						directDiff += attenuatedLight.diffRadiance * X(material, diffReflectance, normalize(attenuatedLight.diffToLight), aNormal);                          \
						directSpec += attenuatedLight.specRadiance * Y(material, specReflectance, normalize(attenuatedLight.specToLight), aNormOut, V, gV, alphaSquared);    \
					}                                                                                                                                                        \
				}                                                                                                                                                            \
				directDiff *= combinedAO;                                                                                                                                    \
				directSpec *= combinedAO;                                                                                                                                    \
			}                                                                                                                                                                \
			DIRECT_SUNLIGHT(X, Y)
	@else
		#define DIRECT_LIGHTING(X, Y) DIRECT_SUNLIGHT(X, Y)
	@endif // !OGFX_NO_LIGHTS

	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	@ifndef OGI_IS_BAKING
		ambientSpec = OW_EnvMap_GetNormalizedSample_1(dominantSpecDir, material.roughness, ambientDiff);
	@endif // OGI_IS_BAKING
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	OW_Lighting(DIRECT_LIGHTING, aAlbedo, material)

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	ambientDiff *= combinedAO;
	ambientSpec *= combinedAO;

@ifndef USE_DEFERRED_DEBUG_FLAGS
	aDebugFlags = vec4(1.0);
@else
	@ifdef OGI_IS_BAKING
		aDebugFlags *= float4(1.0, 1.0, 1.0, 0.0);
	@endif // OGI_IS_BAKING
@endif // USE_DEFERRED_DEBUG_FLAGS

	aReflOut = ambientSpec * aDebugFlags.y;
	aDiffOut = ambientDiff * aDebugFlags.x + directDiff * aDebugFlags.z;

	return (aDiffOut + aReflOut + directSpec * aDebugFlags.w);

	//--------------------------------------------------------------------------------------------------------------------------------------------------
}