import "shaders/modules/graphics/onward/creep/creep_funcs.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int4 OW_Creep_WithinZone(in float3 aPlanetRelPoint, in float aEpsilon)
{
	int4 result = int4(0,0,0,0);

	result.w = 0;

	for (result.x = 0; result.x < OW_Creep_Constants.myNumberOfZones; ++result.x)
	{
		const OW_Creep_Zone zone = OW_Creep_Buffer[result.x];
		float dist = OW_Creep_Get2DDistance(zone.myPlanetRelCenter, aPlanetRelPoint) - aEpsilon;
		if (dist < zone.myRadius)
		{
			++result.w;
			break;
		}
	}

	for (result.y = 0; result.y < OW_Creep_Constants.myNumberOfZones; ++result.y)
	{
		if (result.y <= result.x)
			continue;

		const OW_Creep_Zone zone = OW_Creep_Buffer[result.y];
		float dist = OW_Creep_Get2DDistance(zone.myPlanetRelCenter, aPlanetRelPoint) - aEpsilon;
		if (dist < zone.myRadius)
		{
			++result.w;
			break;
		}
	}

	for (result.z = 0; result.z < OW_Creep_Constants.myNumberOfZones; ++result.z)
	{
		if (result.z <= result.y)
			continue;

		const OW_Creep_Zone zone = OW_Creep_Buffer[result.z];
		float dist = OW_Creep_Get2DDistance(zone.myPlanetRelCenter, aPlanetRelPoint) - aEpsilon;
		if (dist < zone.myRadius)
		{
			++result.w;
			break;
		}
	}

	return result;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
