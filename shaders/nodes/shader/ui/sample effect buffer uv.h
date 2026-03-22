import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/ui/Effect Buffer.h"

float SampleEffectBufferUV(float2 uv)
{
	return MR_SampleLod0(Framebuffer, uv).x;
}
