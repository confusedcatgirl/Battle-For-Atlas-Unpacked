import "shaders/modules/graphics/onward/dithering.h"
import "shaders/nodes/shader/functions/onward/planet/OW_WaterConstants.h"

float OW_WaterDithering(in float aAlpha, float3 aWorldPosition : Gfx_WorldSpacePos, float4 MR_FragCoord : MR_FragCoord)
{
	const float factor = OW_Dithering_GetGlobalFactor(aWorldPosition, OW_TERRAIN_CATEGORY);
	OW_Dithering_CheckAndDiscard(MR_FragCoord.xy, factor);
	return aAlpha;
}
