import "shaders/modules/graphics/environment_constants.h"
import "shaders/nodes/Shader/Functions/Lighting/GI.h"
import "shaders/modules/graphics/lighting_data_struct.h"

//+ ONWARD;
import "shaders/modules/graphics/onworld/onworld_environment.h"
//- ONWARD;

Gfx_DirectionalLight Gfx_GetShadowLight()
{
	Gfx_DirectionalLight light = GetDeprecatedDirectionalLight( );
	light.toLightDirection = Gfx_Environment.shadowLightToLightDirection;
	return light;
}

float3 Gfx_GetEnvironmentGI(float3 normal)
{
    // Use this position to ensure we will use the global values and not the texture
    return SampleGI( normal, float3(99999,99999,99999) );
}

float3 Gfx_Fog3(float3 color, float3 aToCamera, float distance, bool aUseRayleigh,
	Gfx_LightingDataStruct ld, bool aHasClipPos2, float2 aClipPos2, bool aUseFarFog)
{
	return color;
}

float3 Gfx_Fog2(float3 color, float3 aToCamera, float distance, Gfx_LightingDataStruct ld)
{
	return color;
}

float3 Gfx_Fog(float3 color, Gfx_LightingDataStruct ld)
{
	return color;
}
