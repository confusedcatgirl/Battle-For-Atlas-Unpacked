import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/framebuffer_scale.h"

MR_Sampler2D Framebuffer : MR_Framebuffer, 1
{
	filter = point,
	wrap = clamp,
};

float3 GetFramebufferColor(float4 fc : MR_FragCoord)
{
	float2 uv = MR_PixelToFramebufferUV(fc.xy * FramebufferScaleConstants.ColorScale);
	return MR_SampleLod0(Framebuffer, uv).xyz;
}
