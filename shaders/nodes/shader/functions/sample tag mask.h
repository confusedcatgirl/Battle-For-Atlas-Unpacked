

float4 SampleTAGMask(MR_Sampler2D aSampler, float2 uv, in float4 minThreshold, in float4 maxThreshold)
{
	float4 mask = MR_SampleLod0(aSampler, uv);
	//mask = smoothstep( minThreshold, maxThreshold, mask );
	float4 finalMask = float4(0.0f, 0.0f, 0.0f, 0.0f);

	if (mask.x < maxThreshold.x)
		finalMask.x = 1.0f;
	if (mask.y < maxThreshold.y)
		finalMask.y = 1.0f;
	if (mask.z < maxThreshold.z)
		finalMask.z = 1.0f;
	if (mask.w < maxThreshold.w)
		finalMask.w = 1.0f;

	return finalMask * float4(1.0f, 0.0f, 1.0f, 1.0f);
}