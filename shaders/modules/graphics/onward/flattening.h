
struct OGFX_FlatteningParams
{
	float4 myRadii;

	float3 myPlanetCenter;
	int myIsFlatteningActive;

	float3 myOriginGroundPos;
	float  myCurvatureParam1;

	float3 myCenterToOrigin;
	float  myCurvatureParam2;

	float3 myOriginToCenter;
	float  myMaxDelta;

	float4 myFactors;
};
