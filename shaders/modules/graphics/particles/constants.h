constants : MR_ConstantBuffer0
{
	float4x4 ourWorldTransform;
	float4x4 ourWorldToCamera;
	float4x4 ourProjection;
	float4x4 ourWorldToClip;
	float4 ourLightSphere;
	float ourAlpha;
	float ourTime;
	uint ourfreeandnotused;
	uint ourIsIndoor_UseLightVolume;
	float2 ourFragCoordScale;
	float2 ourFragCoordOffset;
	int4 ourBasisSegment;
	float3 ourGISamplePos;
	uint ourGISampleIndex;
	float4 ourPixelConstants[32];
}

const uint Particles_IsIndoorBit = 0x1;
const uint Particles_UseLightVolume = 0x2;

MR_StructuredBuffer<float4x4> MR_ParticleGISamples : MR_ParticleGISamples; // ONWARD: more things

bool Particle_IsIndoor()
{
@ifdef GFX_NO_INDOOR
	return false;
@else
	return (ourIsIndoor_UseLightVolume & Particles_IsIndoorBit) != 0;
@endif // GFX_NO_INDOOR
}

bool Particle_UseLightVolume()
{
	return (ourIsIndoor_UseLightVolume & Particles_UseLightVolume) != 0;
}

float2 ParticleFragCoordToOriginal(float2 fc)
{
	return fc * ourFragCoordScale + ourFragCoordOffset;
}

float2 OriginalToParticleFragCoord(float2 fc)
{
	return ( fc - ourFragCoordOffset ) / ourFragCoordScale;
}
