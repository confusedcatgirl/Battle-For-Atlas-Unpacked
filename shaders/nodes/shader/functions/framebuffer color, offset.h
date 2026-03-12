import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/framebuffer_scale.h"

MR_Sampler2D Framebuffer : MR_Framebuffer, 2
{
	filter = linear point,
	wrap = clamp,
};

float3 GetFramebufferColorOffset(float2 offset, float4 fc : MR_FragCoord)
{
	float2 uv = MR_PixelToFramebufferUV(fc.xy * FramebufferScaleConstants.ColorScale);
	uv += offset;
	return MR_SampleLod0(Framebuffer, clamp(uv, MR_ViewConstants.framebufferUVMin, MR_ViewConstants.framebufferUVMax)).xyz;
}
