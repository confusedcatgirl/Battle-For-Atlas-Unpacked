import "shaders/modules/graphics/gpuparticles/gpuparticles_common.h"
import "shaders/MRender/MR_ViewConstants.h"
import "shaders/nodes/Shader/Functions/Onward/OW_GetTerrainLowResRadius.h"

void CreateParticles(in uint index, in SCommonData CommonData, in MR_StructuredBuffer< SParticleSpawnVolume > SpawnVolumes, inout SParticleData ParticleData)
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

	const uint spawnVolumeIndex = GetSpawnVolumeIndex( index, mySpawnVolsCount);

	pos = SpawnVolumes[spawnVolumeIndex].myPosition;

	if (IsParticleCulled(SpawnVolumes[spawnVolumeIndex].myDistanceToCamera))
	{
		flags |= 32;
	}
	else
	{
		const float arcLengthCorrectionFactor = 1.6f;

		const float3 up = normalize(SpawnVolumes[spawnVolumeIndex].myPosition);
		const float3 forward = abs(up.x) > abs(up.z) ? float3(-up.y, up.x, 0.0) : float3(0.0, -up.z, up.y);
		const float3 right = normalize(cross(up, forward));

		
		pos += (-right * SpawnVolumes[spawnVolumeIndex].myVolumeSize.x + right * SpawnVolumes[spawnVolumeIndex].myVolumeSize.x * 2 * Random01(randomQ)) * arcLengthCorrectionFactor;
		pos += (-forward * SpawnVolumes[spawnVolumeIndex].myVolumeSize.z + forward * SpawnVolumes[spawnVolumeIndex].myVolumeSize.z * 2 * Random01(randomQ)) * arcLengthCorrectionFactor;
		pos = normalize(pos) * (OW_Planet_GetMinTerrainRadius( ) + (Random01(randomQ))*SpawnVolumes[spawnVolumeIndex].myVolumeSize.y*2);
	}

	pos += myPlanetPos;

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