constants In : MR_ConstantBuffer2
{
	float2 destTexelSizeAndUvScale0;
	float2 destTexelSizeAndUvScale1;
	float2 sourceUvOffset0;
	float2 sourceUvOffset1;
	float2 uv0Min;
	float2 uv0Max;
	float2 uv1Min;
	float2 uv1Max;
	int2 destTexelOffset0;
	int2 destTexelOffset1;
}

float2 GetUV0(uint3 DTid)
{
	return (float2(DTid.xy) * In.destTexelSizeAndUvScale0) + In.sourceUvOffset0;
}

float2 GetUV1(uint3 DTid)
{
	return (float2(DTid.xy) * In.destTexelSizeAndUvScale1) + In.sourceUvOffset1;
}