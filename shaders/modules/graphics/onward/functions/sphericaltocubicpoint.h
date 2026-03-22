import "shaders/modules/graphics/onward/functions/SphericalPointToCubicEx.h"

float3 OW_SphericalPointToCubic( in float3 aPos )
{
	float3 ignored;
	return OW_SphericalPointToCubicEx( aPos, aPos, ignored );
}
