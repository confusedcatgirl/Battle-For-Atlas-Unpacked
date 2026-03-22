import "shaders/nodes/Shader/Functions/Onward/Lighting/OW_GetMixedFog.h"

void OW_MixedFog( inout float3 aColor, float3 aFlattenedWorldPosition, float4 MR_FragCoord : MR_FragCoord )
{
	OW_Fog_Apply( aColor, OW_GetMixedFog( aFlattenedWorldPosition, MR_FragCoord ) );
}