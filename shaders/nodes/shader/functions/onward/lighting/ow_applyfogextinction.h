import "shaders/modules/graphics/onward/gi/volumetric_fog.h"

void OW_ApplyFogExtinction( inout float3 aColor, in float4 aFog )
{
	OW_Fog_ApplyExtinction( aColor, aFog.a );
}

