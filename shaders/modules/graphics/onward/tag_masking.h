import "shaders/nodes/Shader/Functions/Apply Planet Transforms To Vertex.h"

constants Hit : MR_ConstantBuffer0
{
	float4x4 myWorldToClip;
	float4x4 myClipToWorld;

	float4 myHitLocation;
	float4 myMaskParams;
	float4 myMaskingMin;
	float4 myMaskingMax;
}

constants Bones : MR_ConstantBuffer1
{
	float4 myTransforms[100];
}

void OW_TAGMask_VertexFunc(in float4 aPosIn, in float2 aUVIn, in float3x4 aObjectToWorldMatrix, out float3 aWSPosOut, out float2 aUVOut, out float4 aClipPosOut)
{
	float4 msPos = float4(aPosIn.xyz * aPosIn.w * 32768, 1);

	aWSPosOut[0] = dot(aObjectToWorldMatrix[0], msPos);
	aWSPosOut[1] = dot(aObjectToWorldMatrix[1], msPos);
	aWSPosOut[2] = dot(aObjectToWorldMatrix[2], msPos);

	//aWSPosOut = ApplyAllPlanetTransformsToVertex(aWSPosOut);

	aClipPosOut = MR_Transform(Hit.myWorldToClip, aWSPosOut);
	aUVOut = aUVIn * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
}