<use4x4Tesselation = true, vertexLighting2 = F"shaders/nodes/Shader/Particles/Apply Vertex Six Point Light.h", vertexLighting2Type = "SixPointFactors">

import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/environment.h"
import "shaders/modules/graphics/shadows.h"
import "shaders/modules/graphics/lights.h"
import "shaders/modules/graphics/lighting_data.h"
import "shaders/modules/graphics/particles/constants.h"
import "shaders/nodes/Shader/Functions/Lighting/GI.h"

struct SixPointFactors
{
	float3 px;
	float3 py;
	float3 pz;
	float3 nx;
	float3 ny;
	float3 nz;
};

global SixPointFactors VertexLighting2Input;

SixPointFactors SixPoint_Init(float px, float py, float pz, float nx, float ny, float nz)
{
	SixPointFactors ret;
	ret.px = vec3(px);
	ret.py = vec3(py);
	ret.pz = vec3(pz);
	ret.nx = vec3(nx);
	ret.ny = vec3(ny);
	ret.nz = vec3(nz);
	return ret;
}

SixPointFactors SixPoint_Init(float3 a)
{
	SixPointFactors ret;
	ret.px = a;
	ret.py = a;
	ret.pz = a;
	ret.nx = a;
	ret.ny = a;
	ret.nz = a;
	return ret;
}

SixPointFactors SixPoint_Init(float3 px, float3 py, float3 pz, float3 nx, float3 ny, float3 nz)
{
	SixPointFactors ret;
	ret.px = px;
	ret.py = py;
	ret.pz = pz;
	ret.nx = nx;
	ret.ny = ny;
	ret.nz = nz;
	return ret;
}

SixPointFactors SixPoint_Add(SixPointFactors a, SixPointFactors b)
{
	SixPointFactors ret = a;
	ret.px += b.px;
	ret.py += b.py;
	ret.pz += b.pz;
	ret.nx += b.nx;
	ret.ny += b.ny;
	ret.nz += b.nz;
	return ret;
}

SixPointFactors SixPoint_Add(SixPointFactors a, float3 b)
{
	SixPointFactors ret = a;
	ret.px += b;
	ret.py += b;
	ret.pz += b;
	ret.nx += b;
	ret.ny += b;
	ret.nz += b;
	return ret;
}

SixPointFactors SixPoint_Mul(SixPointFactors a, float3 b)
{
	SixPointFactors ret = a;
	ret.px *= b;
	ret.py *= b;
	ret.pz *= b;
	ret.nx *= b;
	ret.ny *= b;
	ret.nz *= b;
	return ret;
}

SixPointFactors GetSixPointFactors(float3 dir)
{
	float3 d = normalize(dir);
	float3 posd = saturate(d);
	float3 negd = saturate(-d);
	return SixPoint_Init(posd.x, posd.y, posd.z, negd.x, negd.y, negd.z);
}

struct VertexLightParameters
{
};

void VertexCalculateFactors(inout SixPointFactors aFactorsInOut, Gfx_AttenuatedEmission aEmission,
	Gfx_LightingDataStruct aLD, VertexLightParameters someParameters)
{
	float3 toLight = normalize(aEmission.diffuseToLight);
	SixPointFactors lightFactor = GetSixPointFactors(toLight);
	lightFactor = SixPoint_Mul(lightFactor, aEmission.diffuseEmission);
	aFactorsInOut = SixPoint_Add(aFactorsInOut, lightFactor);
}

vertex void VertexLight2(
	out SixPointFactors aFactorsOut,
	out float4 aFogOut,
	float4 aClipPos,
	float4 aColor
)
{
	aFactorsOut = SixPoint_Init(vec3(0));
	aFogOut = vec4(0);

@ifndef GFX_IS_PARTICLE_SHADOW
	float4 worldPos4 = MR_Transform(MR_ViewConstants.clipToWorld, aClipPos);
	float3 worldPos = worldPos4.xyz / worldPos4.w;

	float2 clip2 = aClipPos.xy / aClipPos.w;
	clip2 = clamp(clip2, vec2(-1), vec2(1));
	float2 fc = MR_ClipToPixel(clip2);

	bool isIndoor = Particle_IsIndoor();

	VertexLightParameters params;
	Gfx_LightingDataStruct ld = Gfx_GetLightingDataDummy(worldPos, fc, aClipPos.w, isIndoor);

	aFactorsOut = SixPoint_Init(SampleGI(float3(1,0,0), worldPos, isIndoor), SampleGI(float3(0,1,0), worldPos, isIndoor), SampleGI(float3(0,0,1), worldPos, isIndoor),
		SampleGI(float3(-1,0,0), worldPos, isIndoor), SampleGI(float3(0,-1,0), worldPos, isIndoor), SampleGI(float3(0,0,-1), worldPos, isIndoor));

//	Gfx_DirectionalLight sun = Onworld_GetShadowLight(worldPos);
//	float3 shadow = GetShadowFactor(worldPos, vec2(0));
//	aFactorsOut = SixPoint_Add(aFactorsOut, SixPoint_Mul(GetSixPointFactors(sun.toLightDirection), sun.emission * shadow));

@include "shaders/modules/graphics/lights_code.h"
	CalculateLighting2(VertexCalculateFactors, aFactorsOut, ld, params);

	float3 vertexToCamera = MR_GetPos(MR_ViewConstants.cameraToWorld).xyz - worldPos;
	float3 fogResult0 = Gfx_Fog3(vec3(0), vertexToCamera, length(vertexToCamera), true, ld, true, clip2, !ld.isIndoor);
	float3 fogResult1 = Gfx_Fog3(vec3(1), vertexToCamera, length(vertexToCamera), true, ld, true, clip2, !ld.isIndoor);

	float diff = fogResult1.x - fogResult0.x;
	float alpha = 1.0 - diff;

	aFogOut.w = alpha;
	aFogOut.xyz = fogResult0 / max(alpha, 0.000001);
@endif
}

void ApplyVertexSixPointLight(
	out float4 aLightOut,
	float4 aPositive,
	float4 aNegative,
	float4 aColor,
	float4 aFog,
	float2 aSixPointUV,
	bool aNormalizeWeights,
	float aContrast <default = 1>,
	float aGamma <default = 1>,
)
{
	aLightOut = float4(0,0,0, aColor.w * aPositive.w);

@ifndef GFX_IS_PARTICLE_SHADOW
	aPositive.xyz = pow(aPositive.xyz, vec3(aGamma));
	aNegative.xyz = pow(aNegative.xyz, vec3(aGamma));

	aPositive.xyz = saturate(lerp(vec3(1.0/6), aPositive.xyz, aContrast));
	aNegative.xyz = saturate(lerp(vec3(1.0/6), aNegative.xyz, aContrast));

	SixPointFactors factors = VertexLighting2Input;

	float2 uvx2 = ddx(aSixPointUV);
	float2 uvy2 = ddy(aSixPointUV);
	float3 uvx = normalize(float3(uvx2, 0));
	float3 uvy = normalize(float3(uvy2, 0));
	float3 uvz = float3(0,0,1);

	float3 tx = uvx;
	float3 ty = uvy;
	float3 tz = uvz;
	uvx = float3(tx.x, -ty.x, tz.x);
	uvy = float3(-tx.y, ty.y, tz.y);
	uvz = float3(tx.z, ty.z, tz.z);

	float3 xaxis = normalize(MR_TransformNormal(MR_ViewConstants.cameraToWorld, uvx));
	float3 yaxis = normalize(MR_TransformNormal(MR_ViewConstants.cameraToWorld, uvy));
	float3 zaxis = -normalize(MR_TransformNormal(MR_ViewConstants.cameraToWorld, uvz));

	float3x3 mat = MR_ColumnMatrix(xaxis, yaxis, zaxis);

	float3 pos2 = MR_Transform(mat, aPositive.xyz);
	float3 neg2 = MR_Transform(mat, -aNegative.xyz);

	float3 pos3 = max(pos2, neg2);
	float3 neg3 = -min(pos2, neg2);

	if (aNormalizeWeights)
	{
		float sum = dot(pos3.xyz, vec3(1)) + dot(neg3.xyz, vec3(1));
		sum = max(1.0, sum);
		pos3.xyz /= sum;
		neg3.xyz /= sum;
	}

	float3 color = factors.px * pos3.x + factors.py * pos3.y + factors.pz * pos3.z
				 + factors.nx * neg3.x + factors.ny * neg3.y + factors.nz * neg3.z;

	aLightOut.xyz = lerp(color * aColor.xyz, aFog.xyz, 0);// TODO
@endif
}
