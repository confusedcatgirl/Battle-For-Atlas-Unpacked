import "shaders/modules/graphics/postfx/chromaticaberration.h"

float3 ChromaticAberrationRadial(float3 aberration, MR_Sampler2D samplerBuffer, float2 uv, float2 uvScale, float2 uvMin, float2 uvMax)
{
	return ChromaticAberrateColorsRadial(aberration, samplerBuffer, uv, uvScale, uvMin, uvMax);
}
