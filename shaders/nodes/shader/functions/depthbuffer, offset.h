import "shaders/MRender/MR_ViewConstants.h"
import "shaders/MRender/MR_DepthBuffer.h"
import "shaders/modules/graphics/framebuffer_scale.h"
import "shaders/modules/graphics/getdepth.h"

float GetDepthbuffer(float2 offset, float4 fc : MR_FragCoord)
{
	float2 totalfc = (offset / MR_ViewConstants.pixelToFramebufferUVScale) + fc.xy;
	return GetDepth(totalfc);
}
