import "shaders/modules/graphics/lights.h"
import "shaders/modules/graphics/lighting_data.h"
import "shaders/modules/graphics/environment.h"
import "shaders/modules/graphics/shadows.h"
import "shaders/MRender/MR_ViewConstants.h"
import "shaders/nodes/Shader/Functions/Lighting/Complete (Transmission, Envmap, Improved Blinn-Phong).h"
import "shaders/nodes/Shader/Functions/Lighting/Color Metalness to Diffuse Spec.h"
import "shaders/nodes/Shader/Functions/Unpack Normal.h"

@ifdef MATERIAL_OVERRIDE
import "shaders/nodes/Shader/Functions/Lighting/Material Override.h"
@endif

constants : MR_ConstantBuffer0
{
	float MaxDistance;
	float StartFadeDistance;
	float3 Color0;
	float3 Color1;
	float4 TerrainLODScaleBias;
	float4 ImagesData[16];
}

vertex in
{
	float4<NATIVE_ARGB_UNORM8> corner;

	stream 1 steprate=1
	{
		float3 position;
		float4<RGBA_UNORM8> normal;
		float4<BGRA_UNORM8> misc;
	}
}

MR_Sampler2D Color : MR_Texture0 { wrap = border };
MR_Sampler2D Normal : MR_Texture1 { wrap = clamp };
MR_Sampler2D Material : MR_Texture2 { wrap = clamp };
MR_Sampler2D TerrainLOD : MR_Texture3 { wrap = clamp };

void GrassTransmissionImprovedBlinnPhong(
	out float3 aLightOut,
	float3 aNormal, float3 aDiffuse, float3 aTransmission,
	float3 aSpec, float aGlossiness, bool aIsTransparent, bool aPointLights <default = true>,
	float aShadowMapZOffset,
	float aAmbientVisibility <default = 1>,
	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData,
	float aDirectionality,
	float3 aGroundNormal
)
{
	float3 vertexToCamera = normalize(Gfx_LightingData.toCamera);

	aLightOut = vec3(0);
	ourIsTransparent = aIsTransparent;
	
@ifndef GFX_DEBUG_NO_SUNLIGHT
	{
		Gfx_DirectionalLight sun = Gfx_GetShadowLight();
		aLightOut = TransmissionImprovedBlinnPhong(aNormal, sun.toLightDirection, vertexToCamera, aDiffuse, aTransmission, aSpec, aGlossiness, sun.emission, aDirectionality);
		aLightOut *= saturate(dot(sun.toLightDirection, aGroundNormal));
		aLightOut *= GetShadowFactor(Gfx_LightingData.worldPosition, Gfx_LightingData.pixelPosition, aShadowMapZOffset);
	}
@endif

@ifndef GFX_DEBUG_NO_POINTLIGHTS
	if (aPointLights)
	{
		Gfx_DirectLight light = Gfx_DiffuseSpec();
		TransmissionImprovedBlinnPhongPointLights(light, vertexToCamera, aNormal, aDiffuse,
			aTransmission, aSpec, aGlossiness, aDirectionality, Gfx_LightingData);

		aLightOut += light.diffuse + light.spec;
	}
@endif
}

void GrassComplete(
	out float3 aLightOut,
	float3 aNormal, float3 aDiffuse, float aOpacity, float3 aSpec, float aRoughness, 
	float aAmbientVisibility <default = 1>,
	float aShadowMapZOffset,
	Gfx_LightingDataStruct Gfx_LightingData : Gfx_LightingData,
	float aDirectionality,
	float3 aGroundNormal
)
{
	aOpacity = saturate(aOpacity);

@ifdef MATERIAL_OVERRIDE
	float useReflection;
	MaterialOverride(aDiffuse, aSpec, aRoughness, aAmbientVisibility, useReflection);
@endif

	float roughnessMod;
	float glossiness;
	RoughnessToGlossiness(aRoughness, roughnessMod, glossiness);

	float3 indirectLight;
	float ssao;
	float ao;
	IndirectLight(indirectLight, ssao, ao, aNormal, aDiffuse, aAmbientVisibility, true, Gfx_LightingData);

	float3 diffuse = aDiffuse * aOpacity;
	float3 transmission = aDiffuse * (1.0 - aOpacity);

	float3 directLight;
	GrassTransmissionImprovedBlinnPhong(directLight, aNormal, diffuse, transmission, aSpec,
		glossiness, true, true, aShadowMapZOffset, ao, Gfx_LightingData, aDirectionality, aGroundNormal);

	aLightOut = indirectLight + directLight;
}

float3 GetTerrainLODColor(float3 aPos)
{
	float2 uv = aPos.xz * TerrainLODScaleBias.xy + TerrainLODScaleBias.zw;
	return MR_SampleLod0(TerrainLOD, uv).xyz;
}
