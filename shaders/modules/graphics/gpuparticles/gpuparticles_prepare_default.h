float CalculateNumToCreate(uint aNumActive, float aPartialCreation)
{
	float numToCreateF = myElapsedTime * mySpawnRate + aPartialCreation;
	return numToCreateF;
}

void PrepareActiveParticles(inout uint numActive, inout uint firstActive, MR_RWStructuredBuffer< SParticleData > someParticleDatas)
{
	while(numActive > 10)
	{
		if((someParticleDatas[(firstActive+10) % myMaxParticles].myStartTime > myCurrentTime - myLifeTime))
			break;
		numActive -= 10;
		firstActive = (firstActive+10) % myMaxParticles;
	}
	while(numActive > 0)
	{
		if((someParticleDatas[firstActive].myStartTime > myCurrentTime - myLifeTime))
			break;
		numActive--;
		firstActive = (firstActive+1) % myMaxParticles;
	}
}