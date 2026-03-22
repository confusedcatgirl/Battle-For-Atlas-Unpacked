import "shaders/MRender/MR_ViewConstants.h"

float4 ClipPosParticles(in float4 aPosPerVertex, in float3 anInstancePos, in float4 someCamSpaceVectors)
{
@ifdef SPACE_2_GROUND
	// float3 mainDir = someCamSpaceVectors.xyz;
	// float3 spherePosition = normalize(anInstancePos);
	// float3 ortoDir = normalize(cross(mainDir, spherePosition));
	float3 offset = float3(0.0f, 1.0f, 0.0f) * aPosPerVertex.y + float3(1.0f, 0.0f, 0.0f) * aPosPerVertex.x; 
@else
	const float3 originToInstancePosCameraSpace = normalize(anInstancePos);
	const float3 rightCameraSpace = someCamSpaceVectors.xyz;
	const float3 upCameraSpace = normalize(cross(originToInstancePosCameraSpace, rightCameraSpace));
	const float3 offset = upCameraSpace * aPosPerVertex.y + rightCameraSpace * aPosPerVertex.x;
@endif

	const float size = someCamSpaceVectors.w;
	const float3 positionCameraSpace = anInstancePos + offset * size;
	return MR_Transform(MR_ViewConstants.cameraToClip, positionCameraSpace);

}

float2 UVParticles(in float4 aPosPerVertex, in float4 anUVOffset)
{
	return ((aPosPerVertex.xy + 1.0f) * 0.5f);
}