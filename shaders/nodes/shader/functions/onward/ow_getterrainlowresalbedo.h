import "shaders/modules/graphics/onward/common/samplers_lookup.h"

// ONLY AVAILABLE IN THE EDITOR... DO NOT PUT PUBLIC
float3 OW_GetTerrainLowResNormal( in float3 aPosition )
{
	return normalize( MR_SampleLod0( LowNormalLookup, aPosition ).xyz * 2.0f - 1.0f );
}
// ONLY AVAILABLE IN THE EDITOR... DO NOT PUT PUBLIC

float3 OW_GetTerrainLowResAlbedo( in float3 aPosition )
{
@ifdef OGI_DEBUG_INTENSITY
	return MR_GRAY_ALBEDO;
@else
	return MR_SampleLod0(LowAlbedoLookup, aPosition).rgb;
@endif // OGI_DEBUG_INTENSITY
}