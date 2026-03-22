import "shaders/modules/graphics/onworld/sun_color.h"
import "shaders/modules/graphics/onworld/sun_light.h"
import "shaders/modules/graphics/onworld/brdf_diffuse.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
float3 OW_Sun_GetDiffuseRadianceOnGround(in float3 aPosition, in float3 aNormal, in float3 aUpVector)
{
	float3 toSunDir = normalize( MR_OnworldEnvironmentConstants.originalDirLightPos - aPosition );
	return OW_Sun_GetSunColor_Relative( aPosition, aNormal, aUpVector, toSunDir ) * OW_BRDF_GetDiffuseTerm( aNormal, toSunDir );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
