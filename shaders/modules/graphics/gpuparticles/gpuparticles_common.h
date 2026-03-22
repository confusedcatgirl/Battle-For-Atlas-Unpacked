import "shaders/modules/graphics/gpuparticles/simplexnoise4d.h"

constants : MR_ConstantBuffer0
{
	float3 myWindVector;
	float myElapsedTime;

	float myInvElapsedTime;
	float myCurrentTime;
	float mySpawnRate;
	float myMinMass;

	float myMaxMass;
	float myMinStreakMass;
	float myMaxStreakMass;
	float myTurbulence;

	float mySize;
	float mySizeMin;
	float mySizeMax;
	uint mySpawnVolsCount;

	float mySpawnCullDistance;
	float myLifeTime;
	uint myMaxParticles;
	float myFree;

	float3 myPlanetPos;
	float myFree2;

	float3 myTintColor;
}


struct SParticleData
{
	float3 myPos;
	float myStartTime;
	float3 myVelocity;
	float myMass;
	float mySize;
	uint myFlags;
};

struct SCommonData
{
	uint myFirstActiveParticle;
	uint myNumActiveParticles;
	uint myFirstParticleToCreate;
	uint myNumParticlesToCreate;
	float myPartialCreation;
};

struct SParticleSpawnVolume
{
	float3 myPosition;
	float3 myVolumeSize;
	float myDistanceToCamera;
};

struct SOutputVertices
{
	float3 myCameraPos;
	uint myTextureIdx;
	float3 myLighting;
	float myAlpha;
	float3 myCameraDir;
	float mySize;
};

uint OldVAXRandom(uint aPreviousNumber)
{
	return (aPreviousNumber * 69069) + 362437;
}

void SetRandomSeed(uint aSeed, inout uint myQ[4])
{
	for (uint i=0; i!=4; ++i)
		myQ[i] = aSeed = (OldVAXRandom(aSeed));
}

uint RandomUInt(inout uint myQ[4])
{
	uint t=(myQ[0]^(myQ[0]<<11));
	myQ[0]=myQ[1];
	myQ[1]=myQ[2];
	myQ[2]=myQ[3];
	myQ[3]=(myQ[3]^(myQ[3]>>19))^(t^(t>>8));
	return myQ[3];
}

float Random01(inout uint myQ[4])
{
	const uint rnd = 0x3F800000 | (RandomUInt(myQ) & 0x7fffff);	// 1.0 .. 1.9999 (with 23 random bits)
	return asfloat(rnd)-1.f;
}

uint GetSpawnVolumeIndex(int particleIndex, uint spawnVolsCount)
{
	return particleIndex % spawnVolsCount;
}

uint GetSpawnPointIndex(int particleIndex, uint spawnPointsCount)
{
	return particleIndex % spawnPointsCount;
}

bool IsParticleCulled(float distanceToCamera)
{
	return distanceToCamera < 0 || distanceToCamera > mySpawnCullDistance;
}

float3 RandomPosNoise(float3 pos, float extraTime)
{
	return float3(snoise(float4(pos, myCurrentTime+extraTime+000), 2, 0.5, 1, 0.5, 1.5), 
				  snoise(float4(pos, myCurrentTime+extraTime+100), 2, 0.5, 1, 0.5, 1.5), 
				  snoise(float4(pos, myCurrentTime+extraTime+200), 2, 0.5, 1, 0.5, 1.5));
}

float3 GetRandomPos(float3 modCameraPos, inout uint randomVal)
{
	uint randomQ[4];

	SetRandomSeed(randomVal,randomQ);
	RandomUInt(randomQ);RandomUInt(randomQ);

	float3 pos = modCameraPos + float3((Random01(randomQ)-0.5)*SnowFieldRange*2, (Random01(randomQ)-0.5)*SnowFieldRangeY*2, (Random01(randomQ)-0.5)*SnowFieldRange*2);
	randomVal = randomVal & 63 | (RandomUInt(randomQ) << 6);
	return pos;
}

float GetRandomValue(inout uint randomVal)
{
	uint randomQ[4];

	SetRandomSeed(randomVal,randomQ);
	RandomUInt(randomQ);RandomUInt(randomQ);

	const float randomValue = (Random01(randomQ)-0.5);

	randomVal = randomVal & 63 | (RandomUInt(randomQ) << 6);

	return randomValue;
}

const float SnowFieldRange = 8;
const float SnowFieldRangeY = 6;