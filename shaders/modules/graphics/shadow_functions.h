
float SampleShadowFactor(MR_Sampler2DCmp aSampler2D, float3 aShadowMapUV)
{
	return MR_SampleLod0Cmp(aSampler2D, aShadowMapUV.xy, aShadowMapUV.z);
}

float SampleShadowFactorArray(MR_Sampler2DArrayCmp aSampler2DArray, float4 aShadowMapUV)
{
	return MR_SampleLod0Cmp(aSampler2DArray, aShadowMapUV.xyz, aShadowMapUV.w);
}

float CalcShadowFactorRandomized(MR_Sampler2DCmp aSampler2D, float3 aShadowMapUV, float2 aOffset, float2 aRotateVec)
{
	float2 rot;
	rot.x = aOffset.x * aRotateVec.x - aOffset.y * aRotateVec.y;
	rot.y = aOffset.y * aRotateVec.x + aOffset.x * aRotateVec.y;

	return SampleShadowFactor(aSampler2D, float3(aShadowMapUV.xy + rot, aShadowMapUV.z));
}

float CalcShadowFactorRandomizedArray(MR_Sampler2DArrayCmp aSampler2DArray, float4 aShadowMapUV, float2 aOffset, float2 aRotateVec)
{
	float2 rot;
	rot.x = aOffset.x * aRotateVec.x - aOffset.y * aRotateVec.y;
	rot.y = aOffset.y * aRotateVec.x + aOffset.x * aRotateVec.y;

	return SampleShadowFactorArray(aSampler2DArray, float4(aShadowMapUV.xy + rot, aShadowMapUV.zw));
}


float CalcShadowFactor4TapRandomized(MR_Sampler2DCmp aSampler2D, float3 aShadowMapUV, float2 aRotateVec, float aOffset)
{
	float samp = 0.0;
	samp = CalcShadowFactorRandomized(aSampler2D, aShadowMapUV, float2(-aOffset, -aOffset), aRotateVec);
	samp += CalcShadowFactorRandomized(aSampler2D, aShadowMapUV, float2(-aOffset, aOffset), aRotateVec);
	samp += CalcShadowFactorRandomized(aSampler2D, aShadowMapUV, float2(aOffset, -aOffset), aRotateVec);
	samp += CalcShadowFactorRandomized(aSampler2D, aShadowMapUV, float2(aOffset, aOffset), aRotateVec);
	samp += CalcShadowFactorRandomized(aSampler2D, aShadowMapUV, float2(0, 0), aRotateVec);
	samp *= 0.2;

	return samp;
}

float CalcShadowFactor4TapRandomizedArray(MR_Sampler2DArrayCmp aSampler2DArray, float4 aShadowMapUV, float2 aRotateVec, float aOffset)
{
	float samp = 0.0;
	samp  = CalcShadowFactorRandomizedArray(aSampler2DArray, aShadowMapUV, float2(-aOffset, -aOffset), aRotateVec);
	samp += CalcShadowFactorRandomizedArray(aSampler2DArray, aShadowMapUV, float2(-aOffset, aOffset), aRotateVec);
	samp += CalcShadowFactorRandomizedArray(aSampler2DArray, aShadowMapUV, float2(aOffset, -aOffset), aRotateVec);
	samp += CalcShadowFactorRandomizedArray(aSampler2DArray, aShadowMapUV, float2(aOffset, aOffset), aRotateVec);
	samp += CalcShadowFactorRandomizedArray(aSampler2DArray, aShadowMapUV, float2(0, 0), aRotateVec);
	samp *= 0.2;

	return samp;
}

float CalcShadowFactor(MR_Sampler2DCmp aSampler2D, float3 aShadowMapUV, float2 aRotateVec, float aOffset)
{
	@ifdef GFX_LOW_QUALITY_SHADOWS
		return SampleShadowFactor(aSampler2D, aShadowMapUV);
	@else
		return CalcShadowFactor4TapRandomized(aSampler2D, aShadowMapUV, aRotateVec, aOffset);
	@endif
}

float CalcShadowFactorArray(MR_Sampler2DArrayCmp aSampler2DArray, float4 aShadowMapUV, float2 aRotateVec, float aOffset)
{
	@ifdef GFX_LOW_QUALITY_SHADOWS
		return SampleShadowFactorArray(aSampler2DArray, aShadowMapUV);
	@else
		return CalcShadowFactor4TapRandomizedArray(aSampler2DArray, aShadowMapUV, aRotateVec, aOffset);
	@endif
}


float GetInsideFactor(float4 aShadowMapCoord)
{
	const float2 splitPosition = float2(0.5, 0.5);
	return max(abs(aShadowMapCoord.x - splitPosition.x), abs(aShadowMapCoord.y - splitPosition.y));
}
