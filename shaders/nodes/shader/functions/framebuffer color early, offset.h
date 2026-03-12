import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/framebuffer_scale.h"

MR_Sampler2D Framebuffer1 : MR_Framebuffer1
{
	filter = linear point,
	wrap = clamp,
};

float3 GetFramebuffer1ColorOffset(float2 offset, float4 fc : MR_FragCoord)
{
	float2 uv = MR_PixelToFramebufferUV(fc.xy * FramebufferScaleConstants.ColorScale);
	uv += offset;
	return MR_SampleLod0(Framebuffer1, clamp(uv, MR_ViewConstants.framebufferUVMin, MR_ViewConstants.framebufferUVMax)).xyz;
}
