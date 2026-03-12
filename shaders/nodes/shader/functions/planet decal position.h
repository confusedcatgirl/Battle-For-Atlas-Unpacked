import "shaders/modules/graphics/onworld/planet_decal.h"

float3 GetPlanetDecalPosition(in float3 aWorldPosition)
{
	aWorldPosition = GetAccuratePlanetDecalPosition( aWorldPosition.xz );
	isFlatteningActive = 0; // also disable flattening
	return aWorldPosition;
}