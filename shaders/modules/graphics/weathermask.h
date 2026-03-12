MR_Texture2D WeatherMask : MR_WeatherMask;

MR_Texture2D<int2> WeatherMaskLookUp : MR_WeatherMaskLookup;

float3 SampleWeatherMaskSingleSample(int2 lookupPos, int2 maskPos)
{
	int2 offsetPos = MR_SampleTexelLod0(WeatherMaskLookUp, lookupPos).rg;
	if(offsetPos.x < 0)
		return float3(0.5f, 0.0f, 0.0f);
	else
		return MR_SampleTexelLod0(WeatherMask, offsetPos + maskPos).rgb;
}