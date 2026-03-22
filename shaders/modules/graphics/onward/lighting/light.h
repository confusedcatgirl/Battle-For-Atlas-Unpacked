import "shaders/modules/graphics/lights.h"

struct OW_AttenuatedLight
{
	float3 diffRadiance;
	float3 specRadiance;

	float3 diffToLight;
	float3 specToLight;
};

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// We just use Division implementation; hopefully this is optimized automatically
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool OW_Light_AttenuateLight(out OW_AttenuatedLight aAttenuatedLight, in Gfx_Light aLight, in float3 aWorldPosition, in float2 aFragCoord)
{
	Gfx_AttenuatedEmission attenuatedEmission = (Gfx_AttenuatedEmission) 0;
	if (Gfx_GetSpotShadowAttenuatedEmission(attenuatedEmission, aLight, aWorldPosition, aFragCoord))
	{
		aAttenuatedLight.diffRadiance = attenuatedEmission.diffuseEmission;
		aAttenuatedLight.diffToLight  = attenuatedEmission.diffuseToLight;

		aAttenuatedLight.specRadiance = attenuatedEmission.specEmission;
		aAttenuatedLight.specToLight  = attenuatedEmission.specToLight;

		return true;
	}
	return false;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OW_Light_SunDisk(inout OW_AttenuatedLight aLight, in float3 aSpecularDir, in float aSubtendedAngle, in float aSurfaceAlpha)
{
	float3 lc2r = dot(aLight.diffToLight, aSpecularDir) * aSpecularDir - aLight.diffToLight;
	aLight.specToLight = normalize(aLight.diffToLight + lc2r * saturate(aSubtendedAngle * rsqrt(dot(lc2r, lc2r))));

	// try to conserve energy; took this from Jarkko, not sure if it's accurate
	float specScale = aSurfaceAlpha / saturate(aSurfaceAlpha + 0.5f * aSubtendedAngle);
	aLight.specRadiance *= specScale * specScale;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// EOF
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
