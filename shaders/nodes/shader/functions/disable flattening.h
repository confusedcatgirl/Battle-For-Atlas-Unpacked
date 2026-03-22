import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/modules/graphics/onworld/planet_flattening.h"

float3 DisableFlattening(in float3 aWorldPosition, in bool aIsMainPlayer)
{
	myIsFlatteningActive = 0;
	return aWorldPosition;
}