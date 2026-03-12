import "shaders/modules/graphics/onward/gi/ground_fog.h"

float4 OW_GetGroundFog( float3 aFlattenedWorldPosition, float4 MR_FragCoord : MR_FragCoord )
{
	return OW_GroundFog_GetFromCoords( OW_GroundFog_GetCoordsFromPos( aFlattenedWorldPosition ) );
}

