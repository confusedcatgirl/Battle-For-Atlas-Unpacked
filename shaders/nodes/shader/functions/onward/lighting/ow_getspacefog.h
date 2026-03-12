import "shaders/modules/graphics/onward/gi/space_fog.h"

float4 OW_GetSpaceFog( in float3 aFlattenedWorldPosition, float4 MR_FragCoord : MR_FragCoord )
{
	return OW_SpaceFog_GetFromCoords( OW_SpaceFog_GetCoordsFromPos( aFlattenedWorldPosition ) );
}