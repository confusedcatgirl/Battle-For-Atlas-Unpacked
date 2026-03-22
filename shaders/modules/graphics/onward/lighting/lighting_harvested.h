import "shaders/modules/graphics/onward/lighting/lighting_opacity.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------
// DIFFUSE
//---------------------------------------------------------------------------------------------------------------------------------------------------

float3 OW_HarvestedLighting_GetDiffReflectance(in float3 aBaseColor, in OGfx_Material aMaterial)
{
	return aBaseColor * (1.0 - aMaterial.metalness);
}

float3 OW_HarvestedLighting_GetDiff(in OGfx_Material aMaterial, in float3 aReflectance, in float3 aL, in float3 aN)
{
	float dotNL = dot(aN, aL);
	aL = (dotNL > 0.0 ? aL : -aL);
	dotNL = saturate(dotNL > 0.0 ? dotNL * aMaterial.opacity : dotNL * aMaterial.opacity - dotNL);

	return OW_DiffBRDF(aReflectance) * dotNL;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// SPECULAR
//---------------------------------------------------------------------------------------------------------------------------------------------------

float3 OW_HarvestedLighting_GetSpecReflectance(in float3 aBaseColor, in OGfx_Material aMaterial)
{
	const float ourDielectricNormalReflectance = 0.04;
	return lerp(ourDielectricNormalReflectance, aBaseColor, aMaterial.metalness);
}

float3 OW_HarvestedLighting_GetSpec(in OGfx_Material aMaterial, in float3 aReflectance, in float3 aL, in float3 aN, in float3 aV)
{
	float dotNL = dot(aN, aL);
	aL = (dotNL > 0.0 ? aL : -aL);
	dotNL = saturate(dotNL > 0.0 ? dotNL * aMaterial.opacity : dotNL * aMaterial.opacity - dotNL);

	const float dotNV = saturate(dot(aN, aV));

	const float3 H = normalize(aL + aV);

	const float dotNH = saturate(dot(aN, H));
	const float dotLH = saturate(dot(aL, H));

	return OW_SpecBRDF_1(aReflectance, dotNL, dotNH, dotLH, dotNV, aMaterial.roughness) * dotNL;
}

// Specialized
float3 OW_HarvestedLighting_GetSpec(in OGfx_Material aMaterial, in float3 aReflectance, in float3 aL, in float3 aN, in float3 aV, in float aGV, in float aAlphaSquared)
{
	float dotNL = dot(aN, aL);
	aL = (dotNL > 0.0 ? aL : -aL);
	dotNL = saturate(dotNL > 0.0 ? dotNL * aMaterial.opacity : dotNL * aMaterial.opacity - dotNL);

	const float3 H = normalize(aL + aV);

	const float dotNH = saturate(dot(aN, H));
	const float dotLH = saturate(dot(aL, H));

	return OW_SpecBRDF_2(aReflectance, dotNL, dotNH, dotLH, aGV, aAlphaSquared) * dotNL;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// REFLECTION
//---------------------------------------------------------------------------------------------------------------------------------------------------