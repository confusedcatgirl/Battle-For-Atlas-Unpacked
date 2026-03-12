import "shaders/nodes/Shader/Functions/Onward/Lighting/OW_GetSpaceFog.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------
// old versions... kept around to make sure old node based shaders are compiling

void OW_SpaceFog( inout float3 aColor, float3 aFlattenedWorldPosition, in float aAdditionalMultiplier <default = 1> )
{
	float4 fog = OW_SpaceFog_GetFromCoords( OW_SpaceFog_GetCoordsFromPos( aFlattenedWorldPosition ) );
	OW_Fog_Apply( aColor, fog );
}

void OW_SpaceFog( inout float3 aColor, float3 aFlattenedWorldPosition, in float aAdditionalMultiplier <default = 1>, float4 MR_FragCoord : MR_FragCoord )
{
	OW_Fog_Apply( aColor, OW_GetSpaceFog( aFlattenedWorldPosition, MR_FragCoord ) );
}

void OW_SpaceFog( inout float3 aColor, float3 aFlattenedWorldPosition )
{
	float4 fog = OW_SpaceFog_GetFromCoords( OW_SpaceFog_GetCoordsFromPos( aFlattenedWorldPosition ) );
	OW_Fog_Apply( aColor, fog );
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

void OW_SpaceFog( inout float3 aColor, float3 aFlattenedWorldPosition, float4 MR_FragCoord : MR_FragCoord )
{
	OW_Fog_Apply( aColor, OW_GetSpaceFog( aFlattenedWorldPosition, MR_FragCoord ) );
}