import "shaders/MRender/MR_ViewConstants.h"

float OW_GetLODDistance(in float3 aWorldPosition)
{
	return abs(dot(MR_ViewConstants.LODDistancePlane.xyz, aWorldPosition) + MR_ViewConstants.LODDistancePlane.w);
}
