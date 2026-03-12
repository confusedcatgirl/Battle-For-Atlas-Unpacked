import "shaders/modules/graphics/gpuparticles/gpuparticles_common.h"
import "shaders/MRender/MR_ViewConstants.h"
import "shaders/nodes/Shader/Functions/Onward/OW_GetTerrainLowResRadius.h"

import "shaders/modules/graphics/onworld/atmosphere/planet_sky.h"

void CreateParticles(in uint index, in SCommonData CommonData, inout SParticleData ParticleData)
{
	uint randomQ[4];
	SetRandomSeed(index,randomQ);

	uint flags = 16;

@ifdef SPACE_2_GROUND
	const float atmosphereRadius = OW_Planet_GetAtmosphereRadius();
	const float playgroundRadius = OW_Planet_GetMinTransitionRegionRadius();
	const float cameraDistance = OW_Planet_GetCameraRadius();
	const float transitionFactor = 1.0f - min(((cameraDistance - playgroundRadius) / (atmosphereRadius - playgroundRadius)), 1.0f);
	const float cloudsPlanePointRadius = lerp(Atm_GetSmoothedRadius(), atmosphereRadius, transitionFactor);

	const float2 pixelPosition = MR_FramebufferUVToPixel(float2(Random01(randomQ), Random01(randomQ)));
	const float2 clipPosition = MR_PixelToClip(pixelPosition);
	const float3 randomPositionPlanetSpace = MR_ClipToWorld(clipPosition, cameraDistance - cloudsPlanePointRadius) - OW_Planet_GetPosition();

	ParticleData.myPos = normalize(randomPositionPlanetSpace);
	ParticleData.myMass = (Random01(randomQ) * 2.0f - 1.0f);

	ParticleData.myVelocity.x = OW_Sky_GetCloudsDensityFromSpaceLod0(ParticleData.myPos);
@else
	flags |= 32;

	ParticleData.myMass = 0.0f;
	ParticleData.myVelocity = 0.0f;
@endif

	ParticleData.myStartTime = myCurrentTime;
	ParticleData.myFlags = flags | (uint)Random01(randomQ) * 8 | (RandomUInt(randomQ) << 6);
	ParticleData.mySize = mySize;
}
