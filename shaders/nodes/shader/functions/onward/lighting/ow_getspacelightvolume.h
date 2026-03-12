import "shaders/modules/graphics/onward/gi/space_fog.h"

float3 OW_GetSpaceLightVolume( float3 aWorldPosition )
{
	OW_FogVolume3DCoords coords = OW_SpaceFog_GetCoordsFromPos( aWorldPosition );
	return MR_SampleLod0( SpaceLightVolume, float3( coords.my2D.myUV, coords.myW ) ).rgb;
}