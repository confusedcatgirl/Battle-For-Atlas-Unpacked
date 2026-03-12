
const float3 YuvWeights = float3(0.299, 0.587, 0.114);

bool IsValid(float f)
{
	return f >= 0 && f < 100000.0;
}

bool IsValid(float3 rgb)
{
	bool3 valid = rgb >= 0 && rgb < 100000.0;
	return valid.x && valid.y && valid.z;
}

float3 RGB2YCoCg(float3 rgb)
{
	float Co = rgb.x - rgb.z;
	float t = rgb.z + Co * 0.5;
	float Cg = rgb.y - t;
	float Y = t + Cg * 0.5;
	return float3(Y, Co, Cg);
}

float3 YCoCg2RGB(float3 YCoCg)
{
	float Y = YCoCg.x;
	float Co = YCoCg.y;
	float Cg = YCoCg.z;

	float t = Y - Cg * 0.5;
	float G = Cg + t;
	float B = t - Co * 0.5;
	float R = Co + B;

	return float3(R, G, B);
}

float getClampFactor(float3 value, float3 start, float3 minValue, float3 maxValue)
{
	float3 delta = value - start;
	float3 maxDelta = maxValue - start;
	float3 minDelta = minValue - start;

	float3 deltaClamp = delta > 0 ? maxDelta : minDelta;

	float3 clampFactors = deltaClamp == 0 ? 0 : delta / deltaClamp;
	float dfmax = max(clampFactors.x, max(clampFactors.y, clampFactors.z));

	return dfmax;
}

float3 clipColorImpl(float3 value, float3 start, float3 minValue, float3 maxValue)
{
	float dfmax = getClampFactor(value, start, minValue, maxValue);

	float3 result;
	if (dfmax > 1)
		result = start + (value - start) / dfmax;
	else
		result = value;

	return clamp(result, minValue, maxValue);
}
