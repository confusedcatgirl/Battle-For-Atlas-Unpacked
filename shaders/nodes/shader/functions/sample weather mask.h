import "shaders/modules/graphics/weathermask.h"

import "shaders/modules/graphics/environment_constants.h"

float3 SampleWeatherMask(float2 uv)
{
	float2 uvInterpolation = uv % Gfx_Environment.weatherMaskTextureResolution.zw;
	float2 uvBase = uv - uvInterpolation;
	uvInterpolation *= Gfx_Environment.weatherMaskTextureResolution.xy;

	uvBase += Gfx_Environment.weatherMaskUvSamplingOffset;

	float2 uvSample0 = uvBase;
	float2 uvSample1 = uvBase + float2(Gfx_Environment.weatherMaskTextureResolution.z, 0.0f);
	float2 uvSample2 = uvBase + float2(0.0f, Gfx_Environment.weatherMaskTextureResolution.w);
	float2 uvSample3 = uvBase + float2(Gfx_Environment.weatherMaskTextureResolution.z, Gfx_Environment.weatherMaskTextureResolution.w);

	int2 lookupPosSample0 = (int2)(uvSample0 * Gfx_Environment.weatherMaskUvTileCount.xy);
	int2 lookupPosSample1 = (int2)(uvSample1 * Gfx_Environment.weatherMaskUvTileCount.xy);
	int2 lookupPosSample2 = (int2)(uvSample2 * Gfx_Environment.weatherMaskUvTileCount.xy);
	int2 lookupPosSample3 = (int2)(uvSample3 * Gfx_Environment.weatherMaskUvTileCount.xy);	

	int2 posSample0 = (int2)(((uvSample0 % Gfx_Environment.weatherMaskUvTileCount.zw) * Gfx_Environment.weatherMaskUvTileCount.xy) * Gfx_Environment.weatherMaskTileResolution);
	int2 posSample1 = (int2)(((uvSample1 % Gfx_Environment.weatherMaskUvTileCount.zw) * Gfx_Environment.weatherMaskUvTileCount.xy) * Gfx_Environment.weatherMaskTileResolution);
	int2 posSample2 = (int2)(((uvSample2 % Gfx_Environment.weatherMaskUvTileCount.zw) * Gfx_Environment.weatherMaskUvTileCount.xy) * Gfx_Environment.weatherMaskTileResolution);
	int2 posSample3 = (int2)(((uvSample3 % Gfx_Environment.weatherMaskUvTileCount.zw) * Gfx_Environment.weatherMaskUvTileCount.xy) * Gfx_Environment.weatherMaskTileResolution);

	float3 sample0 = SampleWeatherMaskSingleSample(lookupPosSample0, posSample0);
	float3 sample1 = SampleWeatherMaskSingleSample(lookupPosSample1, posSample1);
	float3 sample2 = SampleWeatherMaskSingleSample(lookupPosSample2, posSample2);
	float3 sample3 = SampleWeatherMaskSingleSample(lookupPosSample3, posSample3);

	float3 sample4 = lerp(sample0, sample1, uvInterpolation.x);
	float3 sample5 = lerp(sample2, sample3, uvInterpolation.x);

	return lerp(sample4, sample5, uvInterpolation.y);
}
