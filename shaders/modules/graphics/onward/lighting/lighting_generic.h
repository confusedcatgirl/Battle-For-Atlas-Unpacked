import "shaders/modules/graphics/onward/material.h"
import "shaders/modules/graphics/onward/lighting/brdf.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------
// DIFFUSE
//---------------------------------------------------------------------------------------------------------------------------------------------------

float3 OW_GenericLighting_GetDiffReflectance(in float3 aBaseColor, in OGfx_Material aMaterial)
{
	return aBaseColor * (1.0 - aMaterial.metalness);
}

float3 OW_GenericLighting_GetDiff(in OGfx_Material aMaterial, in float3 aReflectance, in float3 aL, in float3 aN)
{
	const float dotNL = saturate(dot(aN, aL));

	return OW_DiffBRDF(aReflectance) * dotNL;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// SPECULAR
//---------------------------------------------------------------------------------------------------------------------------------------------------

float3 OW_GenericLighting_GetSpecReflectance(in float3 aBaseColor, in OGfx_Material aMaterial)
{
	const float ourDielectricNormalReflectance = 0.04;
	return lerp(ourDielectricNormalReflectance, aBaseColor, aMaterial.metalness);
}

float3 OW_GenericLighting_GetSpec(in OGfx_Material aMaterial, in float3 aReflectance, in float3 aL, in float3 aN, in float3 aV)
{
	const float dotNL = saturate(dot(aN, aL));
	const float dotNV = saturate(dot(aN, aV));

	const float3 H = normalize(aL + aV);

	const float dotNH = saturate(dot(aN, H));
	const float dotLH = saturate(dot(aL, H));

	return OW_SpecBRDF_1(aReflectance, dotNL, dotNH, dotLH, dotNV, aMaterial.roughness) * dotNL;
}

// Specialized
float3 OW_GenericLighting_GetSpec(in OGfx_Material aMaterial, in float3 aReflectance, in float3 aL, in float3 aN, in float3 aV, in float aGV, in float aAlphaSquared)
{
	const float dotNL = saturate(dot(aN, aL));

	const float3 H = normalize(aL + aV);

	const float dotNH = saturate(dot(aN, H));
	const float dotLH = saturate(dot(aL, H));

	return OW_SpecBRDF_2(aReflectance, dotNL, dotNH, dotLH, aGV, aAlphaSquared) * dotNL;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// REFLECTION
//---------------------------------------------------------------------------------------------------------------------------------------------------