import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/modules/graphics/onworld/planet_flattening.h"

float3 ApplyAllPlanetTransformsToVertex(in float3 pos)
{
	OW_SFT_ApplyTmp(pos);
	return pos;
}

float3 ApplyPlanetTransformsToVertex(in float3 pos, in bool applyFlattening <default = true>)
{
	if (applyFlattening)
	{
		OW_SFT_ApplyTmp(pos);
	}
	return pos;
}