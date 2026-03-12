import "shaders/nodes/Shader/Functions/Onward/Lighting/OW_GetGroundFog.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
// old versions... kept around to make sure old node based shaders are compiling

void OW_GroundFog( inout float3 aColor, float3 aFlattenedWorldPosition, in float aAdditionalMultiplier <default = 1>, float4 MR_FragCoord : MR_FragCoord )
{
	OW_Fog_Apply( aColor, OW_GetGroundFog( aFlattenedWorldPosition, MR_FragCoord ) );
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

void OW_GroundFog( inout float3 aColor, float3 aFlattenedWorldPosition, float4 MR_FragCoord : MR_FragCoord )
{
	OW_Fog_Apply( aColor, OW_GetGroundFog( aFlattenedWorldPosition, MR_FragCoord ) );
}
