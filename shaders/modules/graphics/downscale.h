
// FYI: max means viewport size - 1 as example :P

float2 SafeClamp(in float2 aSourcePixel, in float2 aMin, in float2 aMax)
{
	return clamp(aSourcePixel, aMin + vec2(0.5), aMax + vec2(0.5));
}

float4 SafeGather4_1(in MR_Sampler2D<float> aSampler, in float2 aSamplerSize, in float2 aViewportPixel, in float2 aViewportMin, in float2 aViewportMax)
{
	if (all(aViewportPixel <= aViewportMax))
	{
		return MR_SampleGather(aSampler, aViewportPixel / aSamplerSize);
	}
	else
	{
		float x = MR_SampleTexelLod0(aSampler, SafeClamp(aViewportPixel + float2(-0.5,  0.5), aViewportMin, aViewportMax)).x;
		float y = MR_SampleTexelLod0(aSampler, SafeClamp(aViewportPixel + float2( 0.5,  0.5), aViewportMin, aViewportMax)).x;
		float z = MR_SampleTexelLod0(aSampler, SafeClamp(aViewportPixel + float2( 0.5, -0.5), aViewportMin, aViewportMax)).x;
		float w = MR_SampleTexelLod0(aSampler, SafeClamp(aViewportPixel + float2(-0.5, -0.5), aViewportMin, aViewportMax)).x;

		return float4(x, y, z, w);
	}
}

float4 SafeGather4_2(in MR_Sampler2D<float2> aSampler, in float2 aSamplerSize, in float2 aViewportPixel, in float2 aViewportMin, in float2 aViewportMax)
{
	if (all(aViewportPixel <= aViewportMax))
	{
		return MR_SampleGather(aSampler, aViewportPixel / aSamplerSize);
	}
	else
	{
		float x = MR_SampleTexelLod0(aSampler, SafeClamp(aViewportPixel + float2(-0.5,  0.5), aViewportMin, aViewportMax)).x;
		float y = MR_SampleTexelLod0(aSampler, SafeClamp(aViewportPixel + float2( 0.5,  0.5), aViewportMin, aViewportMax)).x;
		float z = MR_SampleTexelLod0(aSampler, SafeClamp(aViewportPixel + float2( 0.5, -0.5), aViewportMin, aViewportMax)).x;
		float w = MR_SampleTexelLod0(aSampler, SafeClamp(aViewportPixel + float2(-0.5, -0.5), aViewportMin, aViewportMax)).x;

		return float4(x, y, z, w);
	}
}
