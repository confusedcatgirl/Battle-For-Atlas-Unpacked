import "shaders/modules/graphics/Parallax Mapping.h"

float2 ParallaxMappingWrapper(float2 aUV, MR_Sampler2D aHeight, float aScale, float3x3 TanToWorld, float3 Gfx_WorldSpacePos : Gfx_WorldSpacePos)
{	
	return ParallaxMapping(aUV, aHeight, aScale, TanToWorld, Gfx_WorldSpacePos);
}