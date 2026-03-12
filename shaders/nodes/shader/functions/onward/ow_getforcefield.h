import "shaders/modules/graphics/onward/common/samplers_foliage.h"

float3 OW_GetForceField( in float3 aWorldPosition )
{
	float3 fieldPos = MR_Transform( ForceFieldConstants.ToFieldTransform, float4(aWorldPosition, 1.0f) ).xyz;
	float4 texColor = MR_SampleLod0( ForceFieldLookup, fieldPos.xy );
	return texColor.xyz * saturate( 1.0 - abs( fieldPos.z ) * texColor.a );
}