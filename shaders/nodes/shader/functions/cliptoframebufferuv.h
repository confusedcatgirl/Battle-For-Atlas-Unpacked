import "shaders/MRender/MR_ViewConstants.h"

float2 ClipToFramebufferUV( float2 clip )
{
	return MR_ClipToFramebufferUV(clip);
}
