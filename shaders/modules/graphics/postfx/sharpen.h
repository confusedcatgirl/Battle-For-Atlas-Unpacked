import "shaders/MRender/MR_ViewConstants.h"

float3 Sharpen(MR_Sampler2D aLight, uint2 pixel, float strength, float2 uvmin, float2 uvmax)
{
	float3 center = MR_SampleTexelLod0(aLight, (int2)pixel).xyz;

	float offset = 0.7;
	float2 uv = MR_PixelToFramebufferUV(float2(pixel) + 0.5);
	float dx = MR_ViewConstants.pixelToFramebufferUVScale.x * offset;
	float dy = MR_ViewConstants.pixelToFramebufferUVScale.y * offset;

	float3 sum = MR_SampleLod0(aLight, clamp(uv + float2(dx,-dy), uvmin, uvmax)).xyz;
	sum += MR_SampleLod0(aLight, clamp(uv + float2(-dx,-dy), uvmin, uvmax)).xyz;
	sum += MR_SampleLod0(aLight, clamp(uv + float2(dx,dy), uvmin, uvmax)).xyz;
	sum += MR_SampleLod0(aLight, clamp(uv + float2(-dx,dy), uvmin, uvmax)).xyz;

	return center - (sum * 0.25 - center) * strength;
}
