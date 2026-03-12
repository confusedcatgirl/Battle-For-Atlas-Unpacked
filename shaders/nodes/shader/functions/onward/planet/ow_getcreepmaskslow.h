import "shaders/modules/graphics/onward/creep/creep_funcs.h"

float OW_GetCreepMaskSLOW( in float3 aPosition, in float aInnerEdge, in float aOuterEdge <default = 1>)
{
	float factor = OW_Creep_Constants.myFullyExposedFactor;

	for (int zoneIt = 0; zoneIt < OW_Creep_Constants.myNumberOfZones; ++zoneIt)
	{
		factor = min(factor, OW_Creep_GetZoneGroundFactor(OW_Creep_Buffer[zoneIt], aPosition, aInnerEdge, aOuterEdge));
	}

	return factor;
}
