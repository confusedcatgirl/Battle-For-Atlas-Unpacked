import "shaders/modules/graphics/onward/common/samplers_lookup.h"

float3 ConvertPosToHeightMapTextureArrayUV(in float3 aPos)
{
	aPos = MR_CubeMapCoords(aPos);
	aPos.xy = (aPos.xy * 512.0f - 511.5f) / 513.0f;
	return aPos;
}

float OW_GetTerrainLowResHeight(in float3 aPosition)
{
	float3 uv = ConvertPosToHeightMapTextureArrayUV(aPosition);
	return MR_SampleLod0(LowHeightLookup, uv).x;
}