import "shaders/nodes/Shader/Functions/Lighting/Environment Map.h"
import "shaders/nodes/Shader/Functions/Fresnel-Schlick.h"
import "shaders/MRender/MR_ViewConstants.h"
import "shaders/nodes/Shader/Functions/Lighting/GI.h"
import "shaders/nodes/Shader/Functions/Reflection Direction.h"

float3 GetReflectionFactor(out float aDiffuseFactorOut, float aRoughness, float3 aSpec, float3 aNormal, float3 aVertexToCamera)
{
	float fresnel = saturate(FresnelSchlickFactor(aNormal, aVertexToCamera) * (1 - aRoughness));
	aDiffuseFactorOut = 1 - fresnel;

	return lerp(aSpec, vec3(1), vec3(fresnel));
}

void EnvmapReflectedLight(
	out float3 aLightOut, out float aDiffuseFactorOut,
	float3 aNormal, float3 aDiffuse, float3 aSpec <default = 0.04 0.04 0.04>, float aRoughness,
	float aUseReflection <default = 1>, float aCombinedAO <default = 1>,
	bool aUseCubeMap <default = true>,
	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData
)
{
	aLightOut = vec3(0);
	aDiffuseFactorOut = 1;

@ifndef GFX_DEBUG_NO_ENVMAP
	float3 vertexToCamera = normalize(Gfx_LightingData.toCamera);
	float3 reflected = GetReflectionDirection(-vertexToCamera, aNormal, aRoughness);

	float3 reflectedLight;
	if (aUseCubeMap)
		reflectedLight = SampleEnvMap(reflected, aRoughness, Gfx_LightingData, Gfx_LightingData.isIndoor);
	else
		reflectedLight = SampleGI(reflected, Gfx_LightingData);

	float3 spec = GetReflectionFactor(aDiffuseFactorOut, aRoughness, aSpec, aNormal, vertexToCamera);

	reflectedLight *= aCombinedAO * spec;
	aLightOut += reflectedLight * aUseReflection;
	aDiffuseFactorOut = lerp(1.0, aDiffuseFactorOut, aUseReflection);
@endif
}

void EnvmapReflectedLight(
	out float3 aLightOut,
	float3 aNormal, float3 aDiffuse, float3 aSpec <default = 0.04 0.04 0.04>, float aRoughness,
	float aUseReflection <default = 1>, float aCombinedAO <default = 1>,
	bool aUseCubeMap <default = true>,
	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData
)
{
	float diffuseFactor;
	EnvmapReflectedLight(aLightOut, diffuseFactor,
		aNormal, aDiffuse, aSpec, aRoughness,
		aUseReflection, aCombinedAO,
		aUseCubeMap, Gfx_LightingData);
}

void DoNotUse1()
{
}