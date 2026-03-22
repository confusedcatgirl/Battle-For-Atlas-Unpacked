import "shaders/modules/graphics/onward/batching.h"

constants : MR_ConstantBuffer5
{
	int4   constBasisSegment;
	float4 constFrustum[6];
	float  constNear;
	float  constFar;
	float3 constLODCameraPosition;
	float  constLODScale;

	uint   constInstanceCount;

	uint   constIndexCount;
	uint   constStartIndex;
}
