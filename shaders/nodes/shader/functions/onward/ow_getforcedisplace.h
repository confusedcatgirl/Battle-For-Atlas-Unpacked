import "shaders/modules/graphics/onward/common/samplers_foliage.h"

float OW_GetForceDisplace( in float3 aWorldPosition )
{
	float3 fieldPos = MR_Transform( ForceFieldConstants.ToFieldTransform, float4(aWorldPosition, 1.0f) ).xyz;
	return MR_SampleLod0( ForceFieldLookup, fieldPos.xy ).w;
}