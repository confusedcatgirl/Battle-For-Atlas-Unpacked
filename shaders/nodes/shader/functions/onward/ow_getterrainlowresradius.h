import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/nodes/shader/functions/onward/OW_GetTerrainLowResHeight.h"

float OW_GetTerrainLowResRadius( in float3 aPosition )
{
	return lerp( OW_Planet_GetMinTerrainRadius( ), OW_Planet_GetMaxTerrainRadius( ), OW_GetTerrainLowResHeight( aPosition ) );
}