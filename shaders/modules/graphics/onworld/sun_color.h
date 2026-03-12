import "shaders/modules/graphics/onworld/functions/MiePhase.h"
import "shaders/modules/graphics/onworld/functions/RayPhase.h"

//@ifdef OGI_IS_BAKING
import "shaders/modules/graphics/onworld/gi/sky_sample.h"
//@endif // OGI_IS_BAKING

// @define OGFX_USE_SUN_ATTENUATION

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Attenuation toward the dark size of the planet... basic dot product between the up vector and the sun direction
//---------------------------------------------------------------------------------------------------------------------------------------------------

@ifdef OGFX_USE_SUN_ATTENUATION

float3 OW_Sun_GetAttenuation( in float aUpDotLightsDir )
{
	return saturate( aUpDotLightsDir.xxx );
}

float3 OW_Sun_GetAttenuation( in float3 aSunDir, in float3 aUpVector )
{
	return OW_Sun_GetAttenuation( dot( aUpVector, aSunDir ) );
}

@else

float3 OW_Sun_GetAttenuation( in float aUpDotLightsDir )
{
	return vec3(1.0);
}

float3 OW_Sun_GetAttenuation( in float3 aSunDir, in float3 aUpVector ) 
{
	return vec3(1.0);
}

@endif // OGFX_USE_SUN_ATTENUATION

// Used to fake a sort of atmosphere by darkening everything below clouds from space; lighting is weird if the ground is as bright as the clouds. Kinda make sense since we
// use lighting baked from space. Since we use the transmittance lookup outside, we also scale by the inverse to cancel its effect on the sun light to retrieve the origin.

const float OW_FAKE_PLANET_TRANSMITTANCE_FROM_SPACE = 0.8;

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Return the sun light color... 
//---------------------------------------------------------------------------------------------------------------------------------------------------
float3 OW_Sun_GetSunColor_Relative( in float3 aRelativePosition, in float3 aNormal, in float3 aUpVector, in float3 aSunDir, in float3 anAttenuation )
{
	float3 sunColor;

@ifdef OGI_IS_BAKING_IGNORED
	sunColor = OW_SkyModel_GetSunColor( aRelativePosition, aSunDir );
@else
	@ifndef ISNT_MAIN_PLANET
		sunColor = anAttenuation * MR_OnworldEnvironmentConstants.onPlanetDirLightColor;
	@else
		sunColor = anAttenuation * MR_OnworldEnvironmentConstants.inSpaceSunLightColor;
	@endif // ISNT_MAIN_PLANET
@endif // OGI_IS_BAKING_IGNORED

@ifndef LIGHT_FROM_GROUND

	const float sunIntensityFromSpace = MR_OnworldEnvironmentConstants.inSpaceSunLightIntensity / OW_FAKE_PLANET_TRANSMITTANCE_FROM_SPACE;
	const float3 sunColorFromSpace = OW_SkyModel_GetSunColor( aRelativePosition, aSunDir ) * sunIntensityFromSpace;

	@ifndef ISNT_MAIN_PLANET
		sunColor = lerp( sunColorFromSpace, sunColor, MR_OnworldEnvironmentConstants.groundLerpFactor.x );
	@else
		sunColor = sunColorFromSpace;
	@endif // ISNT_MAIN_PLANET

@endif // LIGHT_FROM_GROUND

	return sunColor;
}

float3 OW_Sun_GetSunColor_Relative( in float3 aRelativePosition, in float3 aNormal, in float3 aUpVector, in float3 aSunDir )
{
	return OW_Sun_GetSunColor_Relative( aRelativePosition, aNormal, aUpVector, aSunDir, OW_Sun_GetAttenuation( aSunDir, aUpVector ) );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// EOF
//---------------------------------------------------------------------------------------------------------------------------------------------------