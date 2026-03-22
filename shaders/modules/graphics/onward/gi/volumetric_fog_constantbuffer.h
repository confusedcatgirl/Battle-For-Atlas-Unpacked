import "shaders/modules/graphics/onward/gi/volumetric_fog_constants.h"

constants OW_FogConstantBuffer : OW_FogConstants
{
	OW_FogEnvConstants myGalaxy;
	OW_FogEnvConstants myPlanet;

	float4x4 myLastFrameMatrix;
}