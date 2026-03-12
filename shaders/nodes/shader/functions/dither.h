import "shaders/modules/graphics/environment_constants.h"
import "shaders/nodes/Shader/Functions/TAA Noise.h"

float Dither(float aAlpha, float4 fc : MR_FragCoord)
{
	float alpha = saturate(aAlpha);
	if (alpha == 0 || alpha == 1)
	{
		return alpha;
	}
	return step(TAANoise(Gfx_Environment.noiseFrame, fc.xy, 3.0), alpha);
}
