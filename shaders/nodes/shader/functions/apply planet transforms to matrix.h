import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/modules/graphics/onworld/planet_flattening.h"

float4x4 ApplyAllPlanetTransformsToMatrix(in float4x4 mat)
{
	OW_SFT_ApplyTmp(mat._m03_m13_m23);
	return mat;
}

float4x4 ApplyPlanetTransformsToMatrix(in float4x4 mat, in bool applyFlattening <default = true>)
{
	if (applyFlattening)
	{
		OW_SFT_ApplyTmp(mat._m03_m13_m23);
	}
	return mat;
}