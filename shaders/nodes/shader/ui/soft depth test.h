import "shaders/modules/graphics/getdepth.h"

pixel float SoftDepthTest(float aRange, float aMinAlpha, float4 aClipPosition : Gfx_ClipPosition, float4 fc : MR_FragCoord)
{
	return GetDepthAsAlpha(fc, aClipPosition.w, aRange, aMinAlpha);
}
