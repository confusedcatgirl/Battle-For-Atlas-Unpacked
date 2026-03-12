import "shaders/modules/graphics/getdepth.h"

pixel float UISoftDepthTestDiscard(float aRange, float aMinAlpha, float4 aClipPosition : Gfx_ClipPosition, float4 fc : MR_FragCoord)
{
	float alpha = GetDepthAsAlpha(fc, aClipPosition.w, aRange, aMinAlpha);
	if (alpha == 0)
		discard;
	return alpha;
}
