import "shaders/modules/graphics/onward/common/samplers_gi.h"

float OW_GetHarvesterTerritoryMask( in float3 aPosition )
{
	return MR_SampleLod0( PlanetGndColorGICubemap, aPosition ).a;
}