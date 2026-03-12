import "shaders/modules/graphics/onward/creep/creep_refs.h"
import "shaders/modules/graphics/environment.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float OW_Creep_Get2DDistance(in float3 aX, in float3 aY, in float aRadius)
{
	return acos(dot(aX, aY) * rsqrt(dot(aX, aX) * dot(aY, aY))) * aRadius;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float OW_Creep_Get2DDistance(in float3 aX, in float3 aY)
{
	return OW_Creep_Get2DDistance(aX, aY, OW_Creep_Constants.myMinPlanetRadius);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float OW_Creep_GetZoneGroundFactor(in OW_Creep_Zone aZone, in float3 aPlanetRelPoint, in float aInnerEdge, in float aOuterEdge)
{
	const float2 groundRadius = float2(aInnerEdge * aZone.myGrowth, aOuterEdge) * aZone.myGrowth * aZone.myRadius;
	float edge = smoothstep(groundRadius.x, groundRadius.y, OW_Creep_Get2DDistance(aPlanetRelPoint, aZone.myPlanetRelCenter));
	return lerp(1.0, edge, aZone.myGrowth);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float OW_Creep_GetZoneGroundFactor(in float3 aPlanetRelPoint, in float aHardness)
{
	float factor = OW_Creep_Constants.myFullyExposedFactor;
	int4 zones = OW_Creep_GetZones(aPlanetRelPoint);
#ifndef IS_ORBIS_TEMP
	[unroll(3)]
#endif
	while(zones.w > 0)
	{
		const OW_Creep_Zone zone = OW_Creep_Buffer[zones[--zones.w]];
		factor = min(factor, OW_Creep_GetZoneGroundFactor(zone, aPlanetRelPoint, aHardness, 1.0));
	}
	return factor;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float4 OW_Creep_GetDecalTexture(in OW_Creep_Zone aZone, in float3 aPlanetRelPoint)
{
	return MR_Sample(OW_Creep_Projector, MR_Transform(aZone.myTextureTransform, aPlanetRelPoint).xy * 16.0);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float OW_Creep_GetZoneGroundFactorWithDecal(in float3 aPlanetRelPoint, bool aIsGround, inout float4 aDecal)
{
	float factor = OW_Creep_Constants.myFullyExposedFactor;
	int4 zones = OW_Creep_GetZones(aPlanetRelPoint);
#ifndef IS_ORBIS_TEMP
	[unroll(3)]
#endif
	while (zones.w > 0)
	{
		const OW_Creep_Zone zone = OW_Creep_Buffer[zones[--zones.w]];
		factor = min(factor, OW_Creep_GetZoneGroundFactor(zone, aPlanetRelPoint, 0.9, 1.0));
		if (aIsGround && factor < 1.0)
		{
			aDecal = lerp(OW_Creep_GetDecalTexture(zone, aPlanetRelPoint), aDecal, factor);
		}
	}
	return factor;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float OW_Creep_GetZoneAirFactor(in OW_Creep_Zone aZone, in float aBaseMask, in float3 aPlanetRelPoint)
{
	float value = OW_Creep_GetZoneGroundFactor(aZone, aPlanetRelPoint, 0.4, 1.0);
	float groundRadius = lerp(OW_Creep_Constants.myMinPlanetRadius, OW_Creep_Constants.myMaxPlanetRadius, abs(aBaseMask));
	float heightAboveGround = max(0.0, length(aPlanetRelPoint) - groundRadius);

	return (1.0 - value) * smoothstep(350.0,250.0,heightAboveGround);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
float OW_Creep_GetZoneAirFactor(in float aBaseMask, in float3 aPlanetRelPoint)
{
	float factor = OW_Creep_Constants.myFullyCoveredFactor;
	int4 zones = OW_Creep_GetZones(aPlanetRelPoint);
#ifndef IS_ORBIS_TEMP
	[unroll(3)]
#endif
	while (zones.w > 0)
	{
		factor = max(factor, OW_Creep_GetZoneAirFactor(OW_Creep_Buffer[zones[--zones.w]], aBaseMask, aPlanetRelPoint));
	}
	return factor;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
