import "shaders/MRender/MR_ViewConstants.h"
import "shaders/MRender/MR_DepthBuffer.h"

float Proximity(float aLength <default = 1.0>, float aFalloffLength <default = 1.0>, float aFalloffPow <default = 16.0>, float3 Gfx_WorldSpacePos : Gfx_WorldSpacePos)
{
	float4 clipPos = MR_Transform(MR_ViewConstants.worldToClip, Gfx_WorldSpacePos);
	float2 uv = MR_ClipToFramebufferUV(clipPos.xy / clipPos.w);

	float depth = MR_SampleLod0(Depthbuffer, uv).x;
	float proximity = pow(max(0.0, 1.0 - max(0.0, depth - clipPos.w - aLength) / aFalloffLength), aFalloffPow);
	
	return saturate(proximity);
}
