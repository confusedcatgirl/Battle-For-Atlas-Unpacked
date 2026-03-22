
constants OW_PlanetConstants : OW_PlanetConstants
{
    float4x4 const_Transform;

    float4 const_PlanetRadii;    // x = crust radius, z = ocean radius, w = atmosphere radius
    float const_AtmSmoothedRadius;
    float const_MaxTransitionRegionRadius;

    float4 const_CameraPos;
    float4 const_CameraDir;

    float4 const_RelativeSunPosition;
    float4 const_PlanetToSunPosition;

    float4 const_PlanetFlags;
    float4 const_Factors;

    float4 const_FlatteningFactors; // to move

    float4 const_CurrentGroundSphere;
    float4 const_FlattenGroundSphere;

	float const_WaterRadius;
	float const_WaterHeight;

    // sky
    float4 const_SkyParams;
    float4 const_SkySphere;
    float4 const_FlattenSkySphere;
    float const_CloudsFade;
    float const_CloudsShadowsFade;
    float const_CloudsEnabled;
    float const_CloudsVelocity;
    float const_CloudsPlanePointRadius;
    float const_CloudsParticlesFade;
    float const_CloudsParticlesAlphaStrength;
    float const_CloudsParticlesSpawningRadius;
    float const_CloudsParticlesSpawningHeightOffset;
    float const_CloudsParticlesDensityCulling;
    float2 const_CloudsLODFadeDistanceRange;
}

// transform functions
float4x4 OW_Planet_GetWorldTransform()         { return OW_PlanetConstants.const_Transform; }
float3   OW_Planet_GetPosition()               { return MR_GetPos(OW_Planet_GetWorldTransform()).xyz; }

float    OW_Planet_GetBaseGroundRadius()       { return OW_PlanetConstants.const_PlanetRadii.x; }
float4   OW_Planet_GetBaseGroundSphere()       { return float4(OW_Planet_GetPosition(), OW_Planet_GetBaseGroundRadius()); }

float4   OW_Planet_GetCurrentGroundSphere()    { return OW_PlanetConstants.const_CurrentGroundSphere; }
float3   OW_Planet_GetCurrentGroundPosition()  { return OW_Planet_GetCurrentGroundSphere().xyz; }
float    OW_Planet_GetCurrentGroundRadius()    { return OW_Planet_GetCurrentGroundSphere().w; }

// camera
float3   OW_Planet_GetCameraDir()              { return OW_PlanetConstants.const_CameraDir.xyz; }
float    OW_Planet_GetCameraHeight()           { return OW_PlanetConstants.const_CameraDir.w;   }
float3   OW_Planet_GetCameraPos()              { return OW_PlanetConstants.const_CameraPos.xyz; }
float    OW_Planet_GetCameraRadius()           { return OW_PlanetConstants.const_CameraPos.w;   }

// flag functions
float    OW_Planet_GetIsMainPlanet()           { return OW_PlanetConstants.const_PlanetFlags.x; }
float    OW_Planet_GetIsBackdrop()             { return OW_PlanetConstants.const_PlanetFlags.y; }

// factor functions

float Planet_GetSmoothingFactor()     { return OW_PlanetConstants.const_FlatteningFactors.z; }
float OW_SFT_GetSmoothingFactor()     { return OW_PlanetConstants.const_FlatteningFactors.z; }
float OW_SFT_GetSmoothedFadeFactor()  { return OW_PlanetConstants.const_FlatteningFactors.w; }

float Planet_GetFlatteningFactor()    { return OW_PlanetConstants.const_FlatteningFactors.x; }
float OW_SFT_GetFlatteningFactor()    { return OW_PlanetConstants.const_FlatteningFactors.x; }
float OW_SFT_GetFlattenedFadeFactor() { return OW_PlanetConstants.const_FlatteningFactors.y; }

float OW_Planet_GetMinTerrainRadius() { return OW_PlanetConstants.const_PlanetRadii.x; }
float OW_Planet_GetMaxTerrainRadius() { return OW_PlanetConstants.const_PlanetRadii.y; }
float OW_Planet_GetMinTransitionRegionRadius() { return OW_PlanetConstants.const_PlanetRadii.z; }
float OW_Planet_GetAtmosphereRadius() { return OW_PlanetConstants.const_PlanetRadii.w; }
float OW_Sky_GetCloudsPlanePointRadius() { return OW_PlanetConstants.const_CloudsPlanePointRadius; }

float OW_Planet_GetMaxTransitionRegionRadius() { return OW_PlanetConstants.const_MaxTransitionRegionRadius; }

float OW_Planet_HeightToRadius(in float aHeight)
{
	return lerp( OW_Planet_GetMinTerrainRadius(), OW_Planet_GetMaxTerrainRadius(), aHeight );
}

// environment

float3 OW_Sun_GetPlanetToSunDir( )
{
	return OW_PlanetConstants.const_PlanetToSunPosition.xyz;
}

float Atm_GetThickness()
{
    return OW_PlanetConstants.const_SkyParams.z;
}

float Atm_GetRadius(in float groundRadius)
{
    return groundRadius + Atm_GetThickness();
}

float Atm_GetSmoothedRadius()
{
    return OW_PlanetConstants.const_AtmSmoothedRadius;
}