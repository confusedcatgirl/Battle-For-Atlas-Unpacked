import "shaders/MRender/MR_ViewConstants.h"

float3 DepthToWorldSpace(float aDepth, float2 offset, float4 fc : MR_FragCoord)
{
	float2 uv = fc.xy;
	uv += offset;
	float3 worldPos = MR_ClipToWorld(MR_PixelToClip(uv), aDepth);
	return worldPos;
}
