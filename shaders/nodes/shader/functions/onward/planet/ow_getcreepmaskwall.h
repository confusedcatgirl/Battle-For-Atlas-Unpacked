import "shaders/modules/graphics/onward/creep/creep_funcs_ex.h"

float OW_GetCreepWall(in float3 aPosition, in float aHardness)
{
	float factor = 1;
	int4 zones = OW_Creep_GetZones(aPosition);
#ifndef IS_ORBIS_TEMP
	[unroll(3)]
#endif
	while(zones.w > 0)
	{
		const OW_Creep_Zone zone = OW_Creep_Buffer[zones[--zones.w]];
		factor = min(factor, OW_Creep_GetZoneGroundFactor(zone, aPosition, aHardness, 1.0));
	}
	return max( factor, OW_Creep_Constants.myFullyCoveredFactor );
}
