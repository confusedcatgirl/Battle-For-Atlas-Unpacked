struct OW_Creep_Zone
{
	float4x4 myTextureTransform;

	float3   myPlanetRelCenter;
	float    myOuterRadius;
	float3   mySegmentRelCenter;
	float    myInnerRadius;

	float    myRadius;
	float    myGrowth;

	float    myUnused1;
	float    myUnused2;
};
