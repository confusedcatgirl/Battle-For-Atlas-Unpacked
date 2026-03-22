import "shaders/modules/graphics/onward/gi/volumetric_fog.h"

void OW_ApplyFog(inout float3 aColor, in float4 aFog)
{
	OW_Fog_Apply( aColor, aFog );
}