import "shaders/MRender/MR_ViewConstants.h"

float4 ClipPosParticles(in float4 aPosPerVertex, in float3 anInstancePos, in float4 someCamSpaceVectors)
{
		float size = someCamSpaceVectors.w;
		float3 mainDir = someCamSpaceVectors.xyz;
		float3 spherePosition = normalize(anInstancePos);
		float3 ortoDir = cross(mainDir, spherePosition) / dot(mainDir, mainDir);
		float3 offset = mainDir * aPosPerVertex.y + ortoDir * aPosPerVertex.x;
		float3 camPos = anInstancePos + offset * size;
		return MR_Transform(MR_ViewConstants.cameraToClip, camPos);
}

float2 UVParticles(in float4 aPosPerVertex, in float4 anUVOffset)
{
	return (aPosPerVertex.xy*(anUVOffset.zw-0.5)+0.5)*float2(.25,.5) + anUVOffset.xy;
}