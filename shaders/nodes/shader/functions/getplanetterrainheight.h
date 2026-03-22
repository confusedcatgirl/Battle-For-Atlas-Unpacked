import "shaders/modules/graphics/onworld/planet_base.h"

float GetPlanetTerrainHeight()
{
	return OW_Planet_GetMaxTerrainRadius() - OW_Planet_GetMinTerrainRadius();
}