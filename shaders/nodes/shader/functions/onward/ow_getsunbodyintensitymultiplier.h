import "shaders/modules/graphics/onworld/env_constants.h"

float3 OW_GetSunBodyIntensityMultiplier( )
{
	return MR_OnworldEnvironmentConstants.groundLerpFactor.yyy;
}