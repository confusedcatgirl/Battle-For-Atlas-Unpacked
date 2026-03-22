float CalculateNumToCreate(uint aNumActive, float aPartialCreation)
{
	float numToCreateF = aNumActive < myMaxParticles? myMaxParticles : 0.0f;
	return numToCreateF;
}

void PrepareActiveParticles(inout uint numActive, inout uint firstActive, MR_RWStructuredBuffer< SParticleData > someParticleDatas)
{
	return;
}