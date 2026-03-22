import "shaders/modules/graphics/onward/creep/creep_funcs_ex.h"

float OW_GetCreepMask(in float3 aPosition, in float aHardness)
{
	return OW_Creep_GetZoneGroundFactor(aPosition, aHardness);
}
