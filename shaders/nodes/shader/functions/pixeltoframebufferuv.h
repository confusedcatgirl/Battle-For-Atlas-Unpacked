import "shaders/MRender/MR_ViewConstants.h"

float2 PixelToFramebufferUV(float2 aPixel)
{
	return MR_PixelToFramebufferUV(aPixel);
}
