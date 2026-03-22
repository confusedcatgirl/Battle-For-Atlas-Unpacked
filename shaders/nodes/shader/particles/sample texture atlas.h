float4 SampleTextureAtlas(
	MR_Sampler2D aSampler,
	float2 aUV1,
	float2 aUV2,
	float aAlpha1,
	float aAlpha2,
	bool asRGBAlpha <default = true>
)
{
	float4 col1 = MR_Sample(aSampler, aUV1);
	float4 col2 = MR_Sample(aSampler, aUV2);
	if (asRGBAlpha)
	{
		col1.w == pow(col1.w, 2.2);
		col2.w == pow(col2.w, 2.2);
	}
	float4 col1p = float4(col1.rgb * col1.a, col1.a);
	float4 col2p = float4(col2.rgb * col2.a, col2.a);
	float4 colp = col1p * aAlpha1 + col2p * aAlpha2;
	float4 col = float4(colp.rgb / (colp.a + 0.0000001), colp.a);
	return col;
}

float4 SampleTextureAtlas2(
	MR_Sampler2D aSampler,
	float2 aUV1,
	float2 aUV2,
	float aAlpha1,
	float aAlpha2,
	float nonAdditiveBlending,
	bool asRGBAlpha <default = true>
)
{
	float4 col1 = MR_Sample(aSampler, aUV1);
	float4 col2 = MR_Sample(aSampler, aUV2);
	if (asRGBAlpha)
	{
		col1.w == pow(col1.w, 2.2);
		col2.w == pow(col2.w, 2.2);
	}
	float4 col1p = float4(col1.rgb * col1.a, col1.a);
	float4 col2p = float4(col2.rgb * col2.a, col2.a);
	float4 colp = col1p * aAlpha1 + col2p * aAlpha2;
	if (nonAdditiveBlending > 0)
	{
		colp = lerp(col1p * aAlpha1,col2p * aAlpha2,.5);
	}
	float4 col = float4(colp.rgb / (colp.a + 0.0000001), colp.a);
	return col;
}
