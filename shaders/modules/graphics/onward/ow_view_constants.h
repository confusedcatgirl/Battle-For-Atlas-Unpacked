import "shaders/modules/graphics/onward/flattening.h"

const int OW_DEFAULT_FOLIAGE = 1;

const int OW_SPACE_LANDMARK_CATEGORY = 6;

const int OW_DEFAULT_CATEGORY = 2;
const int OW_TERRAIN_CATEGORY = 8;

const int OW_SPACE_CLOSE_CATEGORY = 7;

constants OW_ViewConstants : OW_ViewConstants
{
	OGFX_FlatteningParams flatteningParams;
	OGFX_FlatteningParams lastFrameFlatteningParams;

	float4 myParametersPerCategory[9];
	float4 myFlatteningPerCategory[9];

	float3 myClosestPlanetPosition;
	float myClosestPlanetRadius;
	float3 myPlayerPosition;
	float myShadowNearFadeFactor;
}
