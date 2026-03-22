import "shaders/nodes/Shader/Functions/Lighting/GI.h"

float3 OW_GI( float3 aDirection, float3 aFlattenedWorldPosition, float3 aUnflattenedUpVector )
{
	return OW_GI_Sample( aDirection, aFlattenedWorldPosition, aUnflattenedUpVector );
}