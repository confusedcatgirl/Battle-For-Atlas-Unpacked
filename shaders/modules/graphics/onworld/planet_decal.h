import "shaders/modules/graphics/onworld/planet_crust_vs.h"

float3 GetAccuratePlanetDecalPosition(in float2 aPos2D)
{
	float3 worldPosition;

	float4 ignored;
	OW_Planet_VS( aPos2D,
		ignored.xyz,
		ignored.xyz,
		worldPosition,
		ignored.xyz,
		ignored.x,
		ignored.xy,
		ignored.xy,
		ignored.xy);

	return worldPosition;
}