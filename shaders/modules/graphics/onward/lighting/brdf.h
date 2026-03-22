
//---------------------------------------------------------------------------------------------------------------------------------------------------
// DIFFUSE
//---------------------------------------------------------------------------------------------------------------------------------------------------

// Lambert
float3 OW_DiffBRDF(in float3 aDiffReflectance)
{
	return aDiffReflectance * vec3(1.0 / MR_PI);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// SPECULAR
//---------------------------------------------------------------------------------------------------------------------------------------------------

// Distribution function
// GGX

float OW_SpecBRDF_Distribution_2(in float aAlphaSquared, in float aDotNH)
{
	float denom = (aDotNH * aAlphaSquared - aDotNH) * aDotNH + 1.0;
	return aAlphaSquared / (MR_PI * denom * denom);
}

float OW_SpecBRDF_Distribution_1(in float aRoughness, in float aDotNH)
{
	const float alpha = aRoughness * aRoughness;
	const float alphaSquared = alpha * alpha;

	return OW_SpecBRDF_Distribution_2(alphaSquared, aDotNH);
}

// Visibility function
// Smith

float OW_SpecBRDF_Visibility_3(in float aGV, in float aGL)
{
	return rcp(aGV * aGL);
}

float OW_SpecBRDF_Visibility_2(in float aAlphaSquared, in float aDotNL)
{
	return aDotNL + sqrt((aDotNL - aDotNL * aAlphaSquared) * aDotNL + aAlphaSquared);
}

float OW_SpecBRDF_Visibility_1(in float aRoughness, in float aDotNV, in float aDotNL)
{
	const float alpha = aRoughness * aRoughness;
	const float alphaSquared = alpha * alpha;

	const float gv = OW_SpecBRDF_Visibility_2(alphaSquared, aDotNV);
	const float gl = OW_SpecBRDF_Visibility_2(alphaSquared, aDotNL);

	return OW_SpecBRDF_Visibility_3(gv, gl);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------
// Reflectance (Fresnel) function
// Schlick
float3 OW_SpecBRDF_Reflectance_1(in float3 aSpecularReflectance, in float aDotLH)
{
	float factor = ( 1.0 - aDotLH );
	float factorSquared = factor * factor;
	factor = factorSquared * factorSquared * factor;

	return aSpecularReflectance + (vec3(1.0) - aSpecularReflectance) * factor;
}

// BRDF: Cook-Torrance

float3 OW_SpecBRDF_2(in float3 aSpecularReflectance, in float aDotNL, in float aDotNH, in float aDotLH, in float aGV, in float aAlphaSquared)
{
	return OW_SpecBRDF_Distribution_2(aAlphaSquared, aDotNH)
			* OW_SpecBRDF_Visibility_3(aGV, OW_SpecBRDF_Visibility_2(aAlphaSquared, aDotNL))
				* OW_SpecBRDF_Reflectance_1(aSpecularReflectance, aDotLH);
}

float3 OW_SpecBRDF_1(in float3 aSpecularReflectance, in float aDotNL, in float aDotNH, in float aDotLH, in float aDotNV, in float aRoughness)
{
	return OW_SpecBRDF_Distribution_1(aRoughness, aDotNH)
			* OW_SpecBRDF_Visibility_1(aRoughness, aDotNV, aDotNL)
				* OW_SpecBRDF_Reflectance_1(aSpecularReflectance, aDotLH);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// REFLECTION
//---------------------------------------------------------------------------------------------------------------------------------------------------

// This is from Unreal 4, which is also from
// "Getting More Physical in Call of Duty: Black Ops II"
// but using roughness instead of glossiness

float3 OW_EnvBRDF_1(in float3 aSpecularColor, in float aRoughness, in float aDotNV)
{
	const float4 c0 = { -1, -0.0275, -0.572,  0.022 };
	const float4 c1 = {  1,  0.0425,  1.040, -0.040 };

	float4 r = aRoughness * c0 + c1;
	float a004 = min(r.x * r.x, exp2(-9.28 * aDotNV)) * r.x + r.y;
	float2 AB = float2(-1.04, 1.04) * a004 + r.zw;

	return aSpecularColor * AB.x + AB.y;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// EOF
//---------------------------------------------------------------------------------------------------------------------------------------------------
