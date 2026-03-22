import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/ui/Effect Buffer.h"

float SampleEffectBuffer(float4 clip : Gfx_ClipPosition)
{
	float2 uv = MR_ClipToFramebufferUV(clip.xy / clip.w);
	return MR_SampleLod0(Framebuffer, uv).x;
}