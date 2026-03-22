import "shaders/MRender/MR_ViewConstants.h"

MR_Sampler2D Framebuffer : MR_Framebuffer2
{
	filter = linear point,
	wrap = clamp,
};

float GetFramebufferColorOffset(float4 fc : MR_FragCoord)
{
	return MR_SampleTexelLod0(Framebuffer, fc.xy).x;
}
