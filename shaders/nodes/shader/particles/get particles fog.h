import "shaders/modules/graphics/particles/constants.h"

float4 GetParticlesFog()
{
	return MR_ParticleGISamples[ourGISampleIndex][3];
}