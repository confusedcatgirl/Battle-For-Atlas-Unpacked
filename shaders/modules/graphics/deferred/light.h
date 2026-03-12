@include "shaders/modules/graphics/deferred/ssr.h"

import "shaders/MRender/MR_ViewConstants.h"

import "shaders/modules/graphics/deferred_data.h"
import "shaders/modules/graphics/lighting_data.h"
import "shaders/modules/graphics/normal_encode.h"
import "shaders/modules/graphics/screenspace_shadows.h"

import "shaders/nodes/Shader/Functions/Lighting/Color Metalness to Diffuse Spec.h"
import "shaders/nodes/Shader/Functions/Lighting/Complete (Transmission, Envmap, Improved Blinn-Phong).h"
import "shaders/nodes/Shader/Functions/Lighting/Indirect Light (Fill Light).h"

void TransmissionImprovedBlinnPhongCompleteNoFog(
	out float3 aLightOut,
	out float3 aReflectionOut,
	out float3 aDiffuseOut,
	float3 aNormal, float3 aDiffuse, float aOpacity, float3 aSpec, float aRoughness, 
	float aUseReflection <default = 1>, float3 aEmission,
	float aAmbientVisibility <default = 1>, bool aIsTransparent, bool aPointLights <default = true>, bool aUseCubeMap <default = true>,
	float aShadowMapZOffset,
	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData
)
{
	aOpacity = saturate(aOpacity);

	float roughnessMod;
	float glossiness;
	RoughnessToGlossiness(aRoughness, roughnessMod, glossiness);

	float3 indirectLight;
	float ssao;
	float ao;
@ifdef GFX_FILL_LIGHT_GI
	IndirectLightFillLight(indirectLight, ssao, ao, aNormal, aDiffuse, aDiffuse * (1 - aOpacity), aAmbientVisibility, aIsTransparent, Gfx_LightingData);
@else
	ourAllowUseFirstSampleGI = true;
	IndirectLight(indirectLight, ssao, ao, aNormal, aDiffuse, aAmbientVisibility, aIsTransparent, Gfx_LightingData);
@endif
		
	float3 reflectedLight;
	float diffuseFactor;
	EnvmapReflectedLight(reflectedLight, diffuseFactor, aNormal, aDiffuse, aSpec, aRoughness, aUseReflection, ao, aUseCubeMap, Gfx_LightingData);

	float3 diffuse = aDiffuse * diffuseFactor * aOpacity;
	float3 transmission = aDiffuse * diffuseFactor * (1.0 - aOpacity);
	indirectLight *= diffuseFactor;

	Gfx_DirectLight directLight;
	TransmissionImprovedBlinnPhong(directLight, aNormal, diffuse, transmission, aSpec, glossiness,
		aIsTransparent, aPointLights, aShadowMapZOffset, Gfx_LightingData);

	aLightOut = indirectLight + reflectedLight + directLight.diffuse + directLight.spec + aEmission;
	aReflectionOut = reflectedLight;
	aDiffuseOut = indirectLight + directLight.diffuse;
}

bool DeferredComplete(out float3 colorOut, out float3 ssParamOut0, out float3 ssParamOut1, int2 pixel)
{
	float4 mrt0 = MR_SampleTexelLod0(MRT0, pixel);
	float4 mrt1 = MR_SampleTexelLod0(MRT1, pixel);
	float4 mrt2 = MR_SampleTexelLod0(MRT2, pixel);

@ifdef USE_DEPTHBUFFER_Z
	float z = MR_SampleTexelLod0(DepthBufferZ, pixel);
@ifdef GFX_DEFERRED_DEPTH_TEST
	if (z <= 0)
	{
		colorOut = vec3(0);
		return false;
	}
@endif
	float depth = MR_ZToLinearDepth(z);
@else
	float depth = MR_SampleTexelLod0(Depth, pixel).x;
@ifdef GFX_DEFERRED_DEPTH_TEST
	if (depth >= 4000) // TODO: fix
	{
		colorOut = vec3(0);
		return false;
	}
@endif
@endif

	Gfx_DeferredData data;
	Gfx_MRTToDeferred(data, mrt0, mrt1, mrt2);

	ourForceDisableScreenspaceShadows = data.opacity != 1;

	float2 clip2 = MR_PixelToClip(pixel);
	float3 pos = MR_ClipToWorld(clip2, depth);

	Gfx_LightingDataStruct ld = Gfx_GetLightingData(pos, float4(pixel, 0, depth));

	float3 normal = data.worldNormal;

	float3 diffuse, spec;
	float useReflection;
	ColorMetalnessToDiffuseSpec(data.color, data.metalness, data.roughness, 1, data.minReflection, diffuse, spec, useReflection);

	float3 reflection;
	float3 diffuseOut;

	float zoffset = data.opacity < 1 ? -0.0003 : 0;
	TransmissionImprovedBlinnPhongCompleteNoFog(colorOut, reflection, diffuseOut, normal, diffuse, data.opacity, spec, data.roughness, useReflection, vec3(0),
		data.ao, false, true, true, zoffset, ld);


@ifdef SSR_AND_SSS
	ssParamOut0 = reflection;
	ssParamOut1 = diffuseOut;
@else
	ssParamOut1 = vec3(0);
	@ifdef SSR
		ssParamOut0 = reflection;
	@else 
		ssParamOut0 = diffuseOut;
	@endif
@endif

@ifdef COMBINED_FOG
	// This shader is not used anymore
@endif

	return true;
}
