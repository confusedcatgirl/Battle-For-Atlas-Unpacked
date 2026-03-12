import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/modules/graphics/particles/constants.h"

float4 OnwardPlanetParticleMultiplier( float4 aUnfoggedColor, float3 aPosition )
{
	float multiplier = MR_ParticleGISamples[ourGISampleIndex][2].w;
	return float4( aUnfoggedColor.rgb * multiplier, aUnfoggedColor.a);
}