MR_Sampler2D Input : MR_Texture0;

constants : MR_ConstantBuffer0
{
	int2 constSize;

	float2 constInvSize;

	float2 constBlurCenter;

	float constBlurFactor;
	float constBlurRadius;
}

const float SAMPLE_COUNT = 8.0;

void OW_RadialBlur(in float2 anOffset, out float4 aColor)
{
	const float distanceFromCenter = length(anOffset);
	const float2 dirFromCenter = anOffset * rcp(distanceFromCenter);

	const float offRadius = max(0.0, distanceFromCenter - constBlurRadius);
	float2 offsetOffRadius = dirFromCenter * offRadius;

	float2 offsetInRadius = dirFromCenter * min(distanceFromCenter, constBlurRadius);
	float2 offsetInRadiusFromCenter = constBlurCenter + offsetInRadius;

	float scaleParam = constBlurFactor / (1.0 - SAMPLE_COUNT);

	// 1
	float scale = 1.0;
	aColor = MR_SampleLod0(Input, offsetInRadiusFromCenter + offsetOffRadius * scale);

	// 2
	scale += scaleParam;
	aColor += MR_SampleLod0(Input, offsetInRadiusFromCenter + offsetOffRadius * scale);

	// 3
	scale += scaleParam;
	aColor += MR_SampleLod0(Input, offsetInRadiusFromCenter + offsetOffRadius * scale);

	// 4
	scale += scaleParam;
	aColor += MR_SampleLod0(Input, offsetInRadiusFromCenter + offsetOffRadius * scale);

	// 5
	scale += scaleParam;
	aColor += MR_SampleLod0(Input, offsetInRadiusFromCenter + offsetOffRadius * scale);

	// 6
	scale += scaleParam;
	aColor += MR_SampleLod0(Input, offsetInRadiusFromCenter + offsetOffRadius * scale);

	// 7
	scale += scaleParam;
	aColor += MR_SampleLod0(Input, offsetInRadiusFromCenter + offsetOffRadius * scale);

	// 8
	scale += scaleParam;
	aColor += MR_SampleLod0(Input, offsetInRadiusFromCenter + offsetOffRadius * scale);

	aColor /= SAMPLE_COUNT;
}

