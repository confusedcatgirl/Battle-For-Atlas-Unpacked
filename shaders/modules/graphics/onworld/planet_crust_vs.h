import "shaders/MRender/MR_ViewConstants.h"
import "shaders/MRender/MR_DepthBuffer.h"

import "shaders/modules/graphics/onworld/planet.h"
import "shaders/modules/graphics/onworld/planet_flattening.h"
import "shaders/modules/graphics/normal_encode.h"

import "shaders/modules/graphics/onworld/planet_crust_cylindrical_uv.h"

constants : MR_ConstantBuffer0
{
    float4x4 Transform;
    float4 UVTransform;
}

constants : MR_ConstantBuffer3
{
	float3 constCrustRadii;
	float constCrustAlpha;
}

struct CrustGeometryProperties
{
    float height;
    float3 normal;
};

void SetDefaultCrustGeometry(in float3 normal, out CrustGeometryProperties properties)
{
    properties.height = 0.0f;
    properties.normal = normal;
}

MR_Sampler2D LowHeightFromVSShader : MR_Texture59 { filter = linear, wrap = clamp };

void OW_Planet_VS( in float2 pos2D
// results
, out float3 outCamPos
, out float3 outSphereNormal
, out float3 outWorldPos
, out float3 outWorldPosForMapping
, out float2 outUV
, out float2 outUV2
@ifdef CALCULATE_CYLINDERTBN
, out float3 outSphereTangent
, out float3 outSphereBitangent
, out float2 outCylinderUV
@endif // CALCULATE_CYLINDERTBN
)
{
	float3 vertexPosition = float3(pos2D.x, 0.0f, pos2D.y);
	float3 cubePos = MR_Transform(Transform, float4(vertexPosition.xyz, 1)).xyz;

	outUV = float2(UVTransform.x, UVTransform.y) + pos2D  * float2(UVTransform.z, UVTransform.w);
	outUV2 = outUV;
	outUV = (outUV * 512.0f + float2(0.5f,0.5f));
	outUV *= 1.0f / 513.0f;

	float3 worldPos = normalize(cubePos);
	outSphereNormal = worldPos;

@ifdef CALCULATE_CYLINDERTBN
	OW_CalculateCylinderTBN(outSphereNormal, outSphereTangent, outSphereBitangent, outCylinderUV);
@endif

	CrustGeometryProperties crustGeometry;
	SetDefaultCrustGeometry(outSphereNormal, crustGeometry);
	crustGeometry.height = MR_SampleLod(LowHeightFromVSShader, outUV, 0.0).x;

	const float4x4 worldMatrix = OW_Planet_GetWorldTransform();
	float psRadius = lerp(constCrustRadii.x, constCrustRadii.y, crustGeometry.height);
	outWorldPosForMapping.xyz = outSphereNormal * psRadius;
	outWorldPosForMapping.xyz = MR_TransformNormal(worldMatrix, outWorldPosForMapping.xyz);

	float vsRadius = lerp(constCrustRadii.x, EvalSmoothedRadius(constCrustRadii.y, constCrustRadii.z), crustGeometry.height);
	outWorldPos.xyz = outSphereNormal * vsRadius;
	outWorldPos.xyz = MR_Transform(worldMatrix, float4(outWorldPos.xyz, 1.0)).xyz;

@ifdef IS_MAIN_PLANET
	OW_SFT_ApplyAll(outWorldPos.xyz, OW_TERRAIN_CATEGORY);
@endif // IS_MAIN_PLANET

	outCamPos = MR_Transform(MR_ViewConstants.worldToCamera, outWorldPos.xyz).xyz;
}

