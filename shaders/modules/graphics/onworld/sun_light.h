
//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------

void OW_Sun_GetShadowLightDirAndColor( in float3 aWorldPosition, in float3 aRelativePosition, in float3 aNormal, in float3 anUpVector, in float aDepth
, out float3 aPixelToSunDir, out float3 aDirLightDir, out float3 aSunLightColor, out float3 aShadowing, out float3 aPixelToSunDirCloudsShadows, in float3 aWorldAbsolutePosition)
{
	aPixelToSunDirCloudsShadows = 0.0f;

	const float3 lookupDir = normalize(MR_OnworldEnvironmentConstants.originalDirLightPos - aWorldPosition);

@ifdef LIGHT_FROM_GROUND

	@ifndef ISNT_MAIN_PLANET
		aPixelToSunDir = normalize( MR_OnworldEnvironmentConstants.adjustedDirLightPos - aWorldPosition );
		aDirLightDir = MR_OnworldEnvironmentConstants.adjustedDirLightDir;
	@else
		aPixelToSunDir = lookupDir;
		aDirLightDir = MR_OnworldEnvironmentConstants.originalDirLightDir;
	@endif // ISNT_MAIN_PLANET

	aDirLightDir = lerp( aDirLightDir, aPixelToSunDir, saturate(aDepth * MR_OnworldEnvironmentConstants.pixelToSunDirParam1 + MR_OnworldEnvironmentConstants.pixelToSunDirParam2) );
	aShadowing = OW_Sun_GetAttenuation( aDirLightDir, anUpVector );

@else

	aShadowing = vec3(1);

	@ifndef LIGHTING_TRANSITION
		aPixelToSunDir = lookupDir;
		aDirLightDir = MR_OnworldEnvironmentConstants.originalDirLightDir;
	@else
		aPixelToSunDir = normalize( MR_OnworldEnvironmentConstants.adjustedDirLightPos - aWorldPosition );
		aDirLightDir = MR_OnworldEnvironmentConstants.adjustedDirLightDir;
		aPixelToSunDirCloudsShadows = normalize(MR_OnworldEnvironmentConstants.originalDirLightPos - aWorldAbsolutePosition);
	@endif // !LIGHTING_TRANSITION

	aDirLightDir = lerp( aDirLightDir, aPixelToSunDir, saturate(aDepth * MR_OnworldEnvironmentConstants.pixelToSunDirParam1 + MR_OnworldEnvironmentConstants.pixelToSunDirParam2) );

@ifdef LIGHTING_TRANSITION
	aShadowing = OW_Sun_GetAttenuation( aDirLightDir, anUpVector );
@endif // !LIGHTING_TRANSITION

@endif

	aSunLightColor = OW_Sun_GetSunColor_Relative( aRelativePosition, aNormal, anUpVector, lookupDir, aShadowing );
}

//
// Everything that call the following function must be on the main planet (forward, particles,...)
//
void OW_Sun_GetShadowLightDirAndColor( in float3 aWorldPosition, in float3 aNormal, in float3 aUpVector
, out float3 aPixelToSunDir, out float3 aDirLightDir, out float3 aSunLightColor, out float3 aShadowing )
{
	float3 unused = 0;
	float3 unused2 = 0;
	const float depth = length( MR_GetPos( MR_ViewConstants.cameraToWorld ).xyz - aWorldPosition );
	OW_Sun_GetShadowLightDirAndColor( aWorldPosition, aWorldPosition, aNormal, aUpVector, depth, aPixelToSunDir, aDirLightDir, aSunLightColor, aShadowing, unused, unused2);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
