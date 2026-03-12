import "shaders/modules/graphics/postfx/dof.h"

MR_Sampler2D Color0 : MR_Texture0 { filter = linear point, wrap = clamp };
MR_Sampler2D Color1 : MR_Texture1 { filter = linear point, wrap = clamp };

MR_RWTexture2D<float4> Result0 : MR_RW_Texture0;
MR_RWTexture2D<float4> Result1 : MR_RW_Texture1;

constants SeparableBokeh : MR_ConstantBuffer0
{
	float2 UVOffset0;
	float2 UVOffset1;
}

float4 Blur(MR_Sampler2D aSampler, float2 aCenterUV, float2 aMinUV, float2 aMaxUV, float2 aDirection)
{
	float4 center = MR_SampleLod0(aSampler, aCenterUV);

	float centerCoC = center.a;
	float absCenterCoC = abs(centerCoC);

	float weightSum = 0.1;
	float4 sum = center * weightSum;

	int NumSamples = DoF.MaxSamples;
@ifdef SEPERABLE_BOKEH_HALFRES		
	NumSamples = min(NumSamples, int(ceil(absCenterCoC)));
@else
	NumSamples = min(NumSamples, int(ceil(absCenterCoC * 2)));
@endif
	
	for (int i = 0; i < NumSamples; ++i)
	{
		float d = i + 0.5;
	@ifdef SEPERABLE_BOKEH_HALFRES		
		float coc = d;
	@else
		float coc = d * 0.5;
	@endif

		float2 uv = aCenterUV + aDirection * d;
		float2 clampedUV = clamp(uv, aMinUV, aMaxUV);

		float4 c0 = MR_SampleLod0(aSampler, clampedUV);

		float sampleCoC = c0.a;
		float absSampleCoC = abs(sampleCoC);
		
		float testCoC = absCenterCoC;

		float weight = saturate(testCoC - coc) * absSampleCoC;

		sum += c0 * weight;
		weightSum += weight;
	}

	return sum / weightSum;
}
