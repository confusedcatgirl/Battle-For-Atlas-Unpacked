void ChromaticAberrateColorsRadialUV(float3 aberration, float2 uv, float2 uvScale, float2 uvMin, float2 uvMax, out float2 ruv, out float2 guv, out float2 buv)
{
	float2 miduvrange = lerp(uvMin, uvMax, 0.5);
	float2 uvp5range = (uv - miduvrange);
	float scale = sqrt(dot(abs(uvp5range), uvScale));
	float3 scaledAberration = float3(1, 1, 1) + (aberration * scale);
	ruv = clamp(uvp5range * scaledAberration.r + miduvrange, uvMin, uvMax);
	guv = clamp(uvp5range * scaledAberration.g + miduvrange, uvMin, uvMax);
	buv = clamp(uvp5range * scaledAberration.b + miduvrange, uvMin, uvMax);
}

float3 ChromaticAberrateColorsRadial(float3 aberration, MR_Sampler2D Sampler, float2 uv, float2 uvScale, float2 uvMin, float2 uvMax)
{
	float2 ruv;
	float2 guv;
	float2 buv;
	ChromaticAberrateColorsRadialUV(aberration, uv, uvScale, uvMin, uvMax, ruv, guv, buv);

	return float3(MR_SampleLod0(Sampler, ruv).r, MR_SampleLod0(Sampler, guv).g, MR_SampleLod0(Sampler, buv).b);
}