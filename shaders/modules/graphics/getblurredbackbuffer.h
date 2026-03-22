import "shaders/modules/graphics/framebuffer_scale.h"
import "shaders/MRender/MR_ViewConstants.h"

MR_Sampler2D BlurredBackBuffer : MR_BlurredFramebuffer { wrap = clamp };

float3 GetBlurBuffer(float2 fc)	
{
	float2 clip = MR_PixelToClip(fc);
	float2 uv = MR_ClipToQuarterFramebufferUV(clip);
	return MR_SampleLod0(BlurredBackBuffer, uv * FramebufferScaleConstants.NormalScale).xyz;
}
