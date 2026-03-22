import "shaders/nodes/Shader/Functions/Lighting/Environment Map.h"
import "shaders/nodes/Shader/Functions/Fresnel-Schlick.h"
import "shaders/MRender/MR_ViewConstants.h"
import "shaders/nodes/Shader/Functions/Lighting/GI.h"
import "shaders/nodes/Shader/Functions/Reflection Direction.h"

void EnvmapReflectedLightFresnelMask(
	out float3 aLightOut, float aReflectionMask,
	float3 aNormal, float3 aDiffuse, float3 aSpec <default = 0.04 0.04 0.04>, float aRoughness,
	float aUseReflection <default = 1>, float aCombinedAO <default = 1>,
	bool aUseCubeMap <default = true>,
	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData
)
{
	aLightOut = vec3(0);

@ifndef GFX_DEBUG_NO_ENVMAP
	float3 vertexToCamera = normalize(Gfx_LightingData.toCamera);
	float3 reflected = GetReflectionDirection(-vertexToCamera, aNormal, aRoughness);

	float3 reflectedLight;
	if (aUseCubeMap)
		reflectedLight = SampleEnvMap(reflected, aRoughness, Gfx_LightingData, Gfx_LightingData.isIndoor);
	else
		reflectedLight = SampleGI(reflected, Gfx_LightingData);

	float3 spec = lerp(lerp(aSpec,FresnelSchlick(aSpec, aNormal, vertexToCamera), aReflectionMask), aSpec, vec3(0.25 + 0.75 * aRoughness));
	reflectedLight *= aCombinedAO * spec;
	aLightOut += reflectedLight * aUseReflection;
@endif
}

void DoNotUse2()
{
}