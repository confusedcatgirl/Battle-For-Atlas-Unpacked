import "shaders/modules/graphics/onward/gi/ground_fog.h"

float3 OW_GetGroundLightVolume( float3 aFlattenedWorldPosition )
{
	OW_FogVolume3DCoords coords = OW_GroundFog_GetCoordsFromPos( aFlattenedWorldPosition );
	return MR_SampleLod0( PlanetLightVolume, float3( coords.my2D.myUV, coords.myW ) ).rgb;
}