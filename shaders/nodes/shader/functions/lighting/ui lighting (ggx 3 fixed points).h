pixel {
	import "shaders/nodes/Shader/Functions/Fresnel-Schlick.h"
}
struct DirectLightingCtx
{
    float roughness;

    float alpha;
    float alphaSquared;
    float shadowingInv;
};

float SmithGGXInvShadowingTerm(float dotNV, float alphaSquared)
{
    return dotNV + sqrt(alphaSquared + dotNV*dotNV*(1.0f-alphaSquared));
}

void PrepareDirectLighting(inout DirectLightingCtx ctx, float dotNV)
{
    ctx.alpha = ctx.roughness * ctx.roughness;
    ctx.alphaSquared = ctx.alpha * ctx.alpha;

    ctx.shadowingInv = SmithGGXInvShadowingTerm(dotNV, ctx.alphaSquared);
}

float3 Onworld_GetDirectSpecular(in DirectLightingCtx ctx, in float3 specReflectance, in float3 V, in float3 N, float3 L)
{
    const float dotNL = saturate(dot(N, L));
    const float3 H = normalize(L + V);
    const float dotNH = saturate(dot(N, H));

    // distribution function
    float D = max( dotNH * dotNH * (ctx.alphaSquared - 1.0f) + 1.0f, 0.0005f);
    D = ctx.alphaSquared * rcp(3.14159265359f * D * D);

    // compute shadowing term
    float S = rcp(ctx.shadowingInv * SmithGGXInvShadowingTerm(dotNL, ctx.alphaSquared));
    return dotNL * D * S * FresnelSchlick(specReflectance, L, H);
}

float3 Onworld_GetDirectDiffuse(in float3 diffuseAlbedo, in float3 N, in float3 L)
{
    return diffuseAlbedo * saturate(dot(N, L));
}

void UILighting(
	out float3 aLightOut,
	float3 aNormal, float3 aView, float3 aColor, float aMetalness, float aRoughness, float3 aEmission,
	float3 aLight1Position,
	float3 aLight2Position,
	float3 aLight3Position,
	float3 aLight1Color,
	float3 aLight2Color,
	float3 aLight3Color
)
{
	const float dotNV = saturate(dot(aNormal, aView));
    float roughness = aRoughness * 0.8f + 0.05f;
    
    DirectLightingCtx ctx;
    ctx.roughness = roughness;
    PrepareDirectLighting(ctx, dotNV);
    
    float3 diffReflectance = aColor * (1.0f - aMetalness);
    const float ourDielectricNormalReflectance = 0.04f;
    float3 specReflectance = lerp(ourDielectricNormalReflectance, aColor, aMetalness);
	float3 diffuse = float3(0, 0, 0);
	float3 spec = float3(0, 0, 0);
	float3 lightDirections[3] = {aLight1Position, aLight2Position, aLight3Position};
	float3 lightColors[3] = {aLight1Color, aLight2Color, aLight3Color};
	
	for (uint i=0; i < 3; ++i)
    {
		float3 lightDir = lightDirections[i];
		float3 specDir = lightDir;
		float lightHalfConeAngle = 0.7853981633974483f * 0.5f; //45
		float lightHalfConeCos = cos(lightHalfConeAngle);
		float lightRotationToHalfConeSin = sin(lightHalfConeAngle * 0.5f);
		float lightRotationToHalfConeCos = cos(lightHalfConeAngle * 0.5f);
		
		const float3 R = 2.0f * dotNV * aNormal - aView;
		specDir = R;
		if (dot(lightDir, R) < lightHalfConeCos)
		{
			float3 rotAxis = normalize(cross(lightDir, R));
			float4 q = float4(lightRotationToHalfConeSin * rotAxis, lightRotationToHalfConeCos);
			float3 t = 2.0f * cross(q.xyz, lightDir);
			specDir = normalize(lightDir + q.w * t + cross(q.xyz, t));
		}

		diffuse += lightColors[i] * Onworld_GetDirectDiffuse(diffReflectance, aNormal, lightDir);
		spec += lightColors[i] * Onworld_GetDirectSpecular(ctx, specReflectance, aView, aNormal, specDir);
	}
	
	float3 result = diffuse + spec + aEmission;
	aLightOut = result;
}
