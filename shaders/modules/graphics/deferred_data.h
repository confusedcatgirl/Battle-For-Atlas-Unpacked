import "shaders/modules/graphics/normal_encode.h"

struct Gfx_DeferredData
{
	float3 color;
	float3 emission;
	float opacity;
	float minReflection; // unnamed
	float sss; /*not used*/
	float roughness;
	float metalness;
	float ao;
	float3 worldNormal;
};

Gfx_DeferredData Gfx_GetDefaultDeferredData()
{
	Gfx_DeferredData data;
	data.color = vec3(1);
	data.emission = vec3(0);
	data.opacity = 1;
	data.minReflection = 0;
	data.sss = 0;
	data.roughness = 1;
	data.metalness = 0;
	data.ao = 1;
	data.worldNormal = float3(0,1,0);
	return data;
}

const float Gfx_SSSMaxSize = 0.1; // must match Gfx_SSSMaxSize in Gfx_GBuffer.h

void Gfx_DeferredToMRT0(Gfx_DeferredData data, out float4 mrt0)
{
	mrt0 = float4(data.color, data.minReflection);
}

void Gfx_DeferredToMRT1(Gfx_DeferredData data, out float4 mrt1)
{
	mrt1 = float4(Gfx_EncodeNormal_UNorm(data.worldNormal), 0);
}

void Gfx_DeferredToMRT2(Gfx_DeferredData data, out float4 mrt2)
{
	mrt2 = float4(data.roughness, data.metalness, data.opacity, data.ao);
}

void Gfx_DeferredToMRT3(Gfx_DeferredData data, out float4 mrt3)
{
	mrt3 = float4(data.emission, 0);
}

void Gfx_DeferredToMRT(Gfx_DeferredData data, out float4 mrt0, out float4 mrt1, out float4 mrt2)
{
	Gfx_DeferredToMRT0(data, mrt0);
	Gfx_DeferredToMRT1(data, mrt1);
	Gfx_DeferredToMRT2(data, mrt2);
}

void Gfx_DeferredToMRT(Gfx_DeferredData data, out float4 mrt0, out float4 mrt1, out float4 mrt2, out float4 mrt3)
{
	Gfx_DeferredToMRT(data, mrt0, mrt1, mrt2);
	Gfx_DeferredToMRT3(data, mrt3);
}

void Gfx_MRTToDeferred(out Gfx_DeferredData data, float4 mrt0, float4 mrt1, float4 mrt2)
{
	data.color = mrt0.xyz;
	data.worldNormal = Gfx_DecodeNormal_UNorm_Normalized(mrt1.xyz);
	data.roughness = mrt2.x;
	data.metalness = mrt2.y;
	data.opacity = mrt2.z;
	data.sss = 0.0;
	data.ao = mrt2.w;
	data.emission = vec3(0);
	data.minReflection = mrt0.w;
}
