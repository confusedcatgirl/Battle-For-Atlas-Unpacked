float LinearToSRGB( float input )
{
	float result;
	float alpha = 0.055f;
	if (input < 0.0031308)
	{
		result = 12.92f * input;
	}
	else
	{
		result = (1.0f + alpha) * pow(input, 1.0f/2.4f) - alpha;
	}
	return result;
}

float3 LinearToSRGB( float3 input )
{
	return float3( LinearToSRGB(input.x), LinearToSRGB(input.y), LinearToSRGB(input.z) );
}

float sRGBToLinear( float input )
{
	float result;
	float alpha = 0.055f;
	if (input < 0.04045)
	{
		result = input / 12.92f;
	}
	else
	{
		result = pow( (input + alpha) / (1.0f + alpha), 2.4);
	}
	return result;
}

float3 sRGBToLinear( float3 input )
{
	return float3( sRGBToLinear(input.x), sRGBToLinear(input.y), sRGBToLinear(input.z) );
}

void LinearSRGB( float3 input, out float3 linearOut, out float3 sRGBOut)
{
	linearOut = LinearToSRGB(input);
	sRGBOut = sRGBToLinear(input);
}