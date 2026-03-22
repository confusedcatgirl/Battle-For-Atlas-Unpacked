<screenspacedecal = true>

import "shaders/MRender/MR_DepthBuffer.h"
import "shaders/MRender/MR_ViewConstants.h"

float3 ScreenSpaceUv(float4 fc : MR_FragCoord, float3x4 worldToUv)
{
	float depth = MR_SampleLod0(Depthbuffer, MR_PixelToFramebufferUV(fc.xy)).x;
	float3 worldPos = MR_ClipToWorld(MR_PixelToClip(fc.xy), depth);

	float3 uv = MR_Transform(worldToUv, worldPos).xyz;
	float maxOffset = max(abs(uv.x), max(abs(uv.y), abs(uv.z)));
	if(maxOffset > 0.5)
		discard;

	return uv+float3(0.5,0.5,0.5);
}
