import "shaders/modules/graphics/framebuffer_scale.h"
import "shaders/MRender/MR_DepthBuffer.h"
import "shaders/MRender/MR_ViewConstants.h"

float GetDepth(float2 fragCoord)
{
	float2 texel = fragCoord * FramebufferScaleConstants.DepthScale;
	float2 uv = MR_PixelToFramebufferUV(texel);
	return MR_SampleLod0(Depthbuffer, uv).x;
}

pixel float GetDepthAsAlpha(float4 fragCoord, float depthRef, float depthRange, float minAlpha)
{
	float depth = GetDepth(fragCoord.xy);
	return lerp(1.0, minAlpha, saturate((depthRef - depth) / depthRange));
}
