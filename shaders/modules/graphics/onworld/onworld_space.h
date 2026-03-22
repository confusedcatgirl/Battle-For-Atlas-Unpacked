import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/modules/graphics/onworld/env_constants.h"

float3 OW_Space_GetInfluenceRadii(in float aGroundRadius)
{
	return float3(1.0f, 1.1f, 0.1f) * Atm_GetRadius(aGroundRadius);
}

float OW_Space_GetInfluenceFromRadii(float3 radii, float height)
{
	return saturate( (height-radii.x) / radii.z );
}

float OW_Space_GetInfluence(in float aGroundRadius, in float aHeight)
{
	return OW_Space_GetInfluenceFromRadii( OW_Space_GetInfluenceRadii( aGroundRadius ), aHeight );
}