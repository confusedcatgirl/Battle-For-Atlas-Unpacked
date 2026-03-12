import "shaders/modules/graphics/environment_constants.h"

float TAANoise(float frame, float2 fc, float size)
{
	float rcpSize = rcp(size);
	float cycle = floor(frame * rcpSize);

	float t1 = frac((fc.x + fc.y + frame) * rcpSize);
	float t2 = frac((fc.x + cycle) * rcpSize);

	return t1 + t2 * rcpSize;
}

float TAANoiseWithDefault(float aDefault, float4 fc : MR_FragCoord)
{
	float noise = TAANoise(Gfx_Environment.noiseFrame, fc.xy, 3.0);
	return Gfx_Environment.noiseFrame ? noise : aDefault;
}
