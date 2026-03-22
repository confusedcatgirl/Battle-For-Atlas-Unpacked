struct OW_FogEnvConstants
{
	float3  myReflectance;
	float   myNear;

	float   myDistributionRange;
	float   myInvDistributionRange;
	float   myDistributionPower;
	float   myInvDistributionPower;

	float4  myScatteringAbsorption; // extinction = scattering + absorption

	float2  myPadding;
	float   myAnisotropy;
	float   myVisibility;

	float   mySunIntensity;
	float   mySkyInfluence;

	float   myFarExtinction;
	float   myFarScattering;
};
