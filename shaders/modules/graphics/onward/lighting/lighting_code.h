
#define OW_Lighting_Prepare(aMaterial, aN, aV)                                                    \
	                                                                                              \
	const float alpha = aMaterial.roughness * aMaterial.roughness;                                \
	const float alphaSquared = alpha * alpha;                                                     \
	                                                                                              \
	const float dotNV = saturate(dot(aN, aV));                                                    \
	                                                                                              \
	const float gV = OW_SpecBRDF_Visibility_2(alphaSquared, dotNV);                               \
	                                                                                              \
	float3 diffReflectance;                                                                       \
	float3 specReflectance;                                                                       \
	                                                                                              \
	OW_AttenuatedLight attenuatedLight = (OW_AttenuatedLight) 0;                                  \
	                                                                                              \
	float3 _R = ( 2.0 * dotNV * aN ) - aV;                                                        \
	_R = OW_Material_GetDominantSpecDir(_R, aN, aMaterial.roughness, alpha);                      \
	const float3 dominantSpecDir = normalize(_R);                                                 \
	                                                                                              \
	float3 directDiff = vec3(0.0);                                                                \
	float3 directSpec = vec3(0.0);                                                                \
	                                                                                              \
	float3 ambientDiff;                                                                           \
	float3 ambientSpec;                                                                           \
	                                                                                              

#define OW_Lighting(DIRECT_LIGHTING_FUNC, aAlbedo, aMaterial)                                     \
	if (aMaterial.opacity < 1.0)                                                                  \
	{                                                                                             \
		diffReflectance = OW_OpacityLighting_GetDiffReflectance(aAlbedo, aMaterial);              \
		specReflectance = OW_OpacityLighting_GetSpecReflectance(aAlbedo, aMaterial);              \
		                                                                                          \
		ambientDiff *= diffReflectance;                                                           \
		ambientSpec *= OW_EnvBRDF_1(specReflectance, aMaterial.roughness, dotNV);                 \
		                                                                                          \
		DIRECT_LIGHTING_FUNC(OW_OpacityLighting_GetDiff, OW_OpacityLighting_GetSpec)              \
	}                                                                                             \
	else                                                                                          \
	{                                                                                             \
		diffReflectance = OW_GenericLighting_GetDiffReflectance(aAlbedo, aMaterial);              \
		specReflectance = OW_GenericLighting_GetSpecReflectance(aAlbedo, aMaterial);              \
		                                                                                          \
		ambientDiff *= diffReflectance;                                                           \
		ambientSpec *= OW_EnvBRDF_1(specReflectance, aMaterial.roughness, dotNV);                 \
		                                                                                          \
		DIRECT_LIGHTING_FUNC(OW_GenericLighting_GetDiff, OW_GenericLighting_GetSpec)              \
	}
