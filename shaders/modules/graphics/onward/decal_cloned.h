import "shaders/MRender/MR_ViewConstants.h"
import "shaders/MRender/MR_DepthBuffer.h"

import "shaders/modules/graphics/onworld/planet_flattening.h"

constants DecalEntity : MR_ConstantBuffer0
{
	int4 mySegment;

	float4x4 myTransform;
	float4x4 myInverseTransform;
	float4x4 myTextureTransform;

	float4 myColor;
}

constants VisualBones : MR_ConstantBuffer1
{
	float4 myTransforms[100];
}

constants DecalBones : MR_ConstantBuffer2
{
	float4 myTransforms[100];
}

void OW_ClonedDecal_VertexFunc(in float4 aPosIn, in float3x4 aObjectToWorldMatrix, in float3x4 aDecalToWorldMatrix, out float3 aDecalWSPosOut, out float4 aClipPosOut)
{
	float3 wsPos;
	wsPos[0] = dot(aObjectToWorldMatrix[0], aPosIn);
	wsPos[1] = dot(aObjectToWorldMatrix[1], aPosIn);
	wsPos[2] = dot(aObjectToWorldMatrix[2], aPosIn);

	aDecalWSPosOut[0] = dot(aDecalToWorldMatrix[0], aPosIn);
	aDecalWSPosOut[1] = dot(aDecalToWorldMatrix[1], aPosIn);
	aDecalWSPosOut[2] = dot(aDecalToWorldMatrix[2], aPosIn);

	OW_SFT_ApplyTmp(wsPos);
	aClipPosOut = MR_Transform(MR_ViewConstants.worldToClip, wsPos);
}

float3 OW_ClonedDecal_GetUV(in float3 decalWorldPos)
{
	float3 uv = MR_Transform(DecalEntity.myInverseTransform, decalWorldPos).xyz;
	float modelSpace = max(abs(uv.x), max(abs(uv.y), abs(uv.z)));
	if (modelSpace > 0.5)
		discard;

	return MR_Transform(DecalEntity.myTextureTransform, decalWorldPos).xyz;
}