import "shaders/nodes/Shader/Functions/Lighting/Environment Map.h"
import "shaders/nodes/Shader/Functions/Lighting/SS Reflection.h"
import "shaders/nodes/Shader/Functions/Fresnel-Schlick.h"

void SSREnvmapReflectedLight(
	out float3 aLightOut,
	float3 aNormal, float3 aDiffuse, float3 aSpec <default = 0.04 0.04 0.04>, float aRoughness,
	float aUseReflection <default = 1>, float aCombinedAO <default = 1>,
	MR_Sampler2D aFramebufferSampler,
	float aQuality <default = 1>,
	float aRange <default = 1>,
	float aReflectionAlpha <default = 1>,
	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData
)
{
	aLightOut = vec3(0);

@ifndef GFX_DEBUG_NO_ENVMAP
	if (aUseReflection > 0.0)
	{
		float3 vertexToCamera = normalize(Gfx_LightingData.toCamera);
		float3 reflected = reflect(-vertexToCamera, aNormal);

		float3 reflectedLight = SampleEnvMap(reflected, aRoughness, Gfx_LightingData, Gfx_LightingData.isIndoor) * aCombinedAO;

		float ssrMaxRoughness = 0.3;
		float ssrMinFactor = 0.25;
		float4 ssr = vec4(0);
		if (aRoughness < ssrMaxRoughness)
		{
			float t = saturate(aRoughness / ssrMaxRoughness);
			float quality = lerp(2.0, ssrMinFactor, t) * Gfx_Environment.screenSpaceReflectionQuality * aQuality;
			float range = lerp(1.0, ssrMinFactor, t) * aRange;
			float stepFactor = lerp(1.5, 1.0, t);
			ssr = SSR_GetReflection(aFramebufferSampler, Gfx_LightingData.worldPosition, float4(Gfx_LightingData.pixelPosition, 0, Gfx_LightingData.depth), reflected, aNormal, quality, range, stepFactor);

			float alpha = saturate(ssr.a * (1.0 - t) * aReflectionAlpha);
			reflectedLight = lerp(reflectedLight, ssr.rgb, alpha);
		}

		float3 spec = lerp(FresnelSchlick(aSpec, aNormal, vertexToCamera), aSpec, vec3(0.25 + 0.75 * aRoughness));
		reflectedLight *= spec;

		aLightOut = reflectedLight * aUseReflection;
	}
@endif
}

void DoNotUse3()
{
}