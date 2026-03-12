import "shaders/nodes/shader/functions/onward/OW_GetForceField.h"

float OW_Foliage_GetForceFieldCoverage( )
{
	return ForceFieldConstants.Params.x;
}

float OW_Foliage_GetForceFieldHalfCoverage( )
{
	return ForceFieldConstants.Params.y;
}
