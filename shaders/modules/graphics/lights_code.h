#define CalculateLighting2(func, resultinout, lightingData, args)	\
uint baseIndex; 													\
uint numPointlights; 												\
@ifndef GFX_DEBUG_NO_TILED_LIGHTS
{																														\
	float2 minMax; 																										\
	Gfx_GetTileData(baseIndex, numPointlights, minMax, lightingData.pixelPosition); 									\
	if (lightingData.depth >= minMax.x && lightingData.depth <= minMax.y) 												\
	{ 																													\
		for (uint i=0; i<numPointlights; ++i) 																			\
		{ 																												\
@ifdef GFX_LIGHT_FROM_CONSTANT_BUFFER
			uint index = Gfx_IndirectLightIndices.Load((baseIndex + i) * 4);												\
			Gfx_Light tileLight = Gfx_GetLightFromConstantBuffer(index);												\
@else
			Gfx_Light tileLight = Gfx_GetLightTile(baseIndex + i);														\
@endif
			Gfx_AttenuatedEmission attenuatedEmission;																	\
			if (Gfx_LightHitsRooms(tileLight, lightingData.roomBits)													\
				&& Gfx_GetSpotShadowAttenuatedEmission(attenuatedEmission, tileLight, lightingData))					\
				func(resultinout, attenuatedEmission, lightingData, args);												\
		}																												\
	}																													\
}																														\
@endif

#define CalculateLighting2Ideal(func, resultinout, lightingData, normal, args)											\
uint baseIndex; 																										\
uint numPointlights; 																									\
@ifndef GFX_DEBUG_NO_TILED_LIGHTS
{																														\
	float2 minMax; 																										\
	Gfx_GetTileData(baseIndex, numPointlights, minMax, lightingData.pixelPosition); 									\
	if (lightingData.depth >= minMax.x && lightingData.depth <= minMax.y) 												\
	{ 																													\
		for (uint i=0; i<numPointlights; ++i) 																			\
		{ 																												\
@ifdef GFX_LIGHT_FROM_CONSTANT_BUFFER
			uint index = Gfx_IndirectLightIndices.Load((baseIndex + i) * 4);												\
			Gfx_Light tileLight = Gfx_GetLightFromConstantBuffer(index);												\
@else
			Gfx_Light tileLight = Gfx_GetLightTile(baseIndex + i);														\
@endif
			Gfx_AttenuatedEmission attenuatedEmission;																	\
			if (Gfx_LightHitsRooms(tileLight, lightingData.roomBits)													\
				&& Gfx_GetSpotShadowAttenuatedEmissionIdeal(attenuatedEmission, tileLight, lightingData, normal))		\
				func(resultinout, attenuatedEmission, lightingData, args);												\
		}																												\
	}																													\
}																														\
@endif

float GFX_LIGHT_DUMMY;
