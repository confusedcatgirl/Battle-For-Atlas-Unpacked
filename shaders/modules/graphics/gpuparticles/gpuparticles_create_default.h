import "shaders/modules/graphics/gpuparticles/gpuparticles_common.h"
import "shaders/MRender/MR_ViewConstants.h"
import "shaders/nodes/Shader/Functions/Onward/OW_GetTerrainLowResRadius.h"

void CreateParticles(in uint index, in SCommonData CommonData, inout SParticleData ParticleData)
{
	uint randomQ[4];

	SetRandomSeed(index+myCurrentTime*1000,randomQ);
	Random01(randomQ); Random01(randomQ);

	uint flags = Random01(randomQ) * 8;
	float mass = myMinMass+Random01(randomQ)*(myMaxMass-myMinMass);
	if(mass > myMinStreakMass)
		flags |= 16;

	float size = mySize * (0.8+mass*10) * (mySizeMin+(mySizeMax-mySizeMin)*Random01(randomQ)+0.00001);
	float3 pos = 0.0f;

	pos = float3(MR_ViewConstants.cameraToWorld[0][3], MR_ViewConstants.cameraToWorld[1][3], MR_ViewConstants.cameraToWorld[2][3]);
	pos.x += MR_ViewConstants.cameraToWorld[0][2] * SnowFieldRange * 0.8;
	pos.y += MR_ViewConstants.cameraToWorld[1][2] * SnowFieldRangeY * 0.8;
	pos.z += MR_ViewConstants.cameraToWorld[2][2] * SnowFieldRange * 0.8;
	pos += float3((Random01(randomQ)-0.5)*SnowFieldRange*2, (Random01(randomQ)-0.5)*SnowFieldRangeY*2, (Random01(randomQ)-0.5)*SnowFieldRange*2);

	const float3 localwind = myWindVector;
	const float offset = 40.0f; // TODO why is this needed?
	const float height = OW_GetTerrainLowResRadius( pos ) - offset;

	if(height > length(pos - myPlanetPos))
		flags |= 32;

	float3 velocity = localwind * 0.9 + float3(0,-mass*20,0);

	ParticleData.myPos = pos;
	ParticleData.myStartTime = myCurrentTime;
	ParticleData.myFlags = flags | (RandomUInt(randomQ) << 6);
	ParticleData.myMass = mass;
	ParticleData.mySize = size;
	ParticleData.myVelocity = velocity;
}