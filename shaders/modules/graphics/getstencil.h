import "shaders/modules/graphics/framebuffer_scale.h"
import "shaders/MRender/MR_StencilTexture.h"

uint GetStencil(float2 fragCoord)
{
// legacy support:
#define Sampler_stencil StencilTexture

	float2 texel = fragCoord * FramebufferScaleConstants.StencilScale;
	return MR_SampleStencilTexelLod0(StencilTexture, texel);
}

void SoftTestStencil(uint stencilValue, float4 fragCoord)
{
	uint stencil = GetStencil(fragCoord.xy);
	if(stencil == stencilValue)
		discard;
}

void DiscardIfStencilContainsBits(uint maskValue, float4 fragCoord)
{
	uint stencil = GetStencil(fragCoord.xy);
	if((stencil & maskValue) != 0)
		discard;
}
