import "shaders/modules/graphics/onworld/functions/HorizonFactors.h"
import "shaders/nodes/Shader/Functions/CartesianToSphericalCoords.h"
import "shaders/modules/graphics/hlsl_utils.h"
import "shaders/modules/graphics/onworld/env_constants.h"
import "shaders/modules/graphics/onworld/functions_sphere.h"
import "shaders/modules/graphics/onworld/functions_plane.h"
import "shaders/modules/graphics/shadows.h"
@ifdef OGFX_DEFERRED_COMPUTE
import "shaders/nodes/Shader/Functions/Onward/OW_SampleEquirectangularTexture.h"
@else
import "shaders/nodes/Shader/Functions/Onward/OW_SampleEquirectangularTextureNoSeams.h"
@endif // OGFX_DEFERRED_COMPUTE
import "shaders/modules/graphics/onworld/atmosphere/bruneton/atmosphere.h"

import "shaders/nodes/Shader/Functions/Onward/Lighting/OW_SpaceGI.h"

MR_Sampler2D FromSpaceCloudsTexture : OW_SkyCloudsTextures0;
MR_SamplerCube NegCloudsCube : OW_SkyCloudsTextures1 { filter = linear, wrap = clamp };
MR_SamplerCube PosCloudsCube : OW_SkyCloudsTextures2 { filter = linear, wrap = clamp };

// @define PAUSE_CLOUDS

float4 OW_Sky_SampleEquirectangularTextureLod0(MR_Sampler2D aSampler, in float3 aDirection)
{
	float4 uv;
	uv.x = atan2( aDirection.z, aDirection.x );
	uv.x += MR_PI * 2.0 * step( uv.x, 0.0 );
	uv.yw = acos( aDirection.y );

	uv.z = atan2( aDirection.z, abs( aDirection.x ) );

	uv /= float4( MR_PI * 2.0, MR_PI, MR_PI * 2.0, MR_PI );
	return MR_SampleLod0( aSampler, uv.xy);
}

float4 OW_Sky_GetBaseDomeColor_Relative( in float4 aHorizonFactors, in float4 aZenithColor, in float4 aHorizonColor )
{
	return lerp( aZenithColor, aHorizonColor, aHorizonFactors );
}

float4 OW_Sky_GetBaseDomeColor_Relative( in float3 aEyePos, in float3 aEyeDir, in float aRadius,
											in float4 aZenithColor, in float4 aHorizonColor, in float aShift, in float aDecay )
{
	float4 horizonFactors = OW_HorizonFactors_Relative( aEyePos, aEyeDir, aRadius, aShift.xxxx, aDecay.xxxx );
	return OW_Sky_GetBaseDomeColor_Relative( horizonFactors, aZenithColor, aHorizonColor );
}

float3 OW_Sky_GetCloudUVWs(in float3 aViewDir)
{
	return aViewDir;
}

float3 OW_Sky_CloudsFlowTransformForce(in float3 aSamplingDir, in float t)
{
	const float velocity = OW_PlanetConstants.const_CloudsVelocity;
	const float rotationAngle = t * velocity;
	return MR_RotateOnYAxis(aSamplingDir, rotationAngle);
}

float3 OW_Sky_CloudsFlowTransform(in float3 aSamplingDir, in float t)
{
@ifdef OGI_IS_BAKING
	// Don't animate clouds while baking
	return aSamplingDir;
@else
@ifdef PAUSE_CLOUDS
	return aSamplingDir;
@else
	const float velocity = OW_PlanetConstants.const_CloudsVelocity;
	const float rotationAngle = t * velocity;
	return MR_RotateOnYAxis(aSamplingDir, rotationAngle);
@endif // PAUSE_CLOUDS
@endif // OGI_IS_BAKING
}

float3 OW_Sky_CloudsFlowTransform(in float3 aSamplingDir)
{
	return OW_Sky_CloudsFlowTransform(aSamplingDir, Gfx_Environment.time);
}

float3 OW_Sky_CloudsFlowInvTransform(in float3 aSamplingDir)
{
	return OW_Sky_CloudsFlowTransform(aSamplingDir, -Gfx_Environment.time);
}

float2 OW_Sky_SampleFromSpaceClouds(in float3 aDirection)
{
@ifdef OGFX_DEFERRED_COMPUTE
	return OW_Sky_SampleEquirectangularTextureLod0(
@else
	return OW_SampleEquirectangularTextureNoSeams(
@endif // OGFX_DEFERRED_COMPUTE
		FromSpaceCloudsTexture, OW_Sky_CloudsFlowTransform(aDirection)).xy;
}

float2 OW_Sky_SampleFromSpaceCloudsLod0(in float3 aDirection)
{
	return OW_Sky_SampleEquirectangularTextureLod0(FromSpaceCloudsTexture, OW_Sky_CloudsFlowTransform(aDirection)).xy;
}

float4 OW_Sky_ProcessCloudsSample(in float4 aSample)
{
	return aSample;
}

float4 OW_Sky_SampleCloudCoveragePOS(in float3 uvw)
{
	return OW_Sky_ProcessCloudsSample(MR_Sample( PosCloudsCube, uvw ));
}

float4 OW_Sky_SampleCloudCoverageNEG(in float3 uvw)
{
	return OW_Sky_ProcessCloudsSample(MR_Sample( NegCloudsCube, uvw ));
}

float4 OW_Sky_SampleCloudCoverageNEGLod0(in float3 uvw)
{
	return OW_Sky_ProcessCloudsSample(MR_SampleLod0( NegCloudsCube, uvw ));
}

float OW_Sky_GetCloudsDensityFromSpaceLod0(in float3 aRelativePosition)
{
@ifdef CLOUDS_ENABLED
	const float3 lookUpVector = normalize( aRelativePosition );

	return OW_Sky_SampleFromSpaceCloudsLod0( lookUpVector ).g;
@else
	return 0.0f;
@endif // CLOUDS_ENABLED
}

float4 OW_Sky_GetCloudsColorFromSpace(float3 origin, float3 direction, float3 anAbsolutePosition, float3 aRelativePosition, float atmosphereBrightness)
{
@ifdef CLOUDS_ENABLED

	const float3 lookUpVector = normalize( aRelativePosition );
	float cloudsColorFactor = OW_Sky_SampleFromSpaceClouds( lookUpVector ).g;

	const float3 sunDirection = OW_Sun_GetPlanetToSunDir( );

	const float dotLS = saturate( dot( lookUpVector, sunDirection ) );

	float3 ambient = OW_Space_GI( lookUpVector, anAbsolutePosition );
	float3 direct = MR_OnworldEnvironmentConstants.inSpaceSunLightIntensity * dotLS;
//	direct *= GetShadowFactor( anAbsolutePosition, vec2(0) );

	const float dotLookDir = saturate(dot(lookUpVector, -direction));
	const float cloudsFadingOnPlanetsEdgeFactor = smoothstep(0.1f, 0.38f, dotLookDir);

	float4 cloudsColor = 0.0f;
	cloudsColor.a = cloudsColorFactor * cloudsFadingOnPlanetsEdgeFactor * OW_PlanetConstants.const_CloudsFade;
	cloudsColor.rgb = (direct + ambient);

	return cloudsColor;

@else
	return 0.0f;
@endif // CLOUDS_ENABLED
}

float3 OW_Sky_GetCloudsColorFromSpace_Particles(float3 anAbsolutePosition)
{
@ifdef CLOUDS_ENABLED

	const float3 lookUpVector = normalize( anAbsolutePosition - OW_Planet_GetPosition());

	const float3 sunDirection = OW_Sun_GetPlanetToSunDir( );

	const float dotLS = saturate( dot( lookUpVector, sunDirection ) );

	float3 ambient = OW_Space_GI( lookUpVector, anAbsolutePosition );
	float3 direct = MR_OnworldEnvironmentConstants.inSpaceSunLightIntensity * dotLS;
//	direct *= GetShadowFactor( anAbsolutePosition, vec2(0) );

	return direct + ambient;

@else
	return 0.0f;
@endif // CLOUDS_ENABLED
}

float4 OW_Sky_BlendAtmosphereAndClouds( in float4 aAtmosphereColor, in float4 aCloudsColor )
{
	float4 color = 0.0f;
	color.a = saturate(1.0f - aCloudsColor.a);
	color.rgb = aCloudsColor.rgb + color.a * aAtmosphereColor.rgb;
	return color;
}

float OW_Sky_GetRayleighPhase(float aCos2)
{
	return (3.0 / 4.0) * (1.0 + aCos2);
}

float OW_Sky_GetMiePhase(float aCos, float aCos2, float aG)
{
	float g2 = aG * aG;
	return (3.0*(1.0-g2))/(2.0*(2.0+g2)) * ((1.0 + aCos2) / pow(1.0 + g2 - 2.0 * aG * aCos, 1.5));
}

float4 OW_Sky_DrawAxis(in float3 aDir)
{
	if (aDir.x > 0.999f)
		return float4(50.0f, 0.0f, 0.0f, 0.0f);

	if (aDir.g > 0.999f)
		return float4(0.0f, 50.0f, 0.0f, 0.0f);

	if (aDir.b > 0.999f)
		return float4(0.0f, 0.0f, 50.0f, 0.0f);

	if (aDir.x < -0.99999f)
		return float4(50.0f, 0.0f, 0.0f, 0.0f);

	if (aDir.g < -0.99999f)
		return float4(0.0f, 50.0f, 0.0f, 0.0f);

	if (aDir.b < -0.99999f)
		return float4(0.0f, 0.0f, 50.0f, 0.0f);

	return 0.0f;
}

void OW_Sky_CloudsSamplingDir(in float3 aViewOrigin, in float3 aViewDir, out float3 aSamplingDir, out float anIntersectionDistance, out float3 anIntersectionPoint)
{
	const float planePointRadius = OW_Sky_GetCloudsPlanePointRadius();
	const float3 planeNormal = normalize(aViewOrigin);
	anIntersectionDistance = Plane_RayIntersect(aViewOrigin, aViewDir, planeNormal, planeNormal * planePointRadius);

	anIntersectionPoint = aViewOrigin + aViewDir * anIntersectionDistance;

	aSamplingDir = OW_Sky_CloudsFlowTransform(normalize(anIntersectionPoint));
}

float OW_Sky_NormalizeIntersectionDistance(float anIntersectionDistance)
{
	return saturate((1.0f - (max(0.0f, anIntersectionDistance) / 65504.0f)));
}

float OW_Sky_GetCloudsShadowsFromInside( float3 anOrigin, float3 aDirection )
{
	float3 samplingDir = aDirection;

	float intersectionDistance;
	float3 intersectionPoint;

	OW_Sky_CloudsSamplingDir( anOrigin, aDirection,
		samplingDir, intersectionDistance, intersectionPoint );

	const float3 uvW = OW_Sky_GetCloudUVWs( samplingDir );
	return 1.0f - MR_Sample( NegCloudsCube, uvW ).w;
}

float OW_Sky_GetCloudsShadowsFromInside( float3 anOrigin, float3 aDirection, float aDepth )
{
	float3 samplingDir = aDirection;

	{
		const float planePointRadius = OW_Planet_GetAtmosphereRadius();
		const float3 planeNormal = normalize(anOrigin);
		float intersectionDistance = Plane_RayIntersect(anOrigin, aDirection, planeNormal, planeNormal * planePointRadius);

		float3 intersectionPoint = anOrigin + aDirection * intersectionDistance;
		samplingDir = OW_Sky_CloudsFlowTransform(normalize(intersectionPoint));
	}

	const float3 uvW = OW_Sky_GetCloudUVWs( samplingDir );
	return max(1.0f - MR_SampleLod( NegCloudsCube, uvW, max( 0.0, log2( 2.0f + 0.005f * aDepth ) ) ).w, 0.15f);
}

float OW_Sky_GetCloudsShadowsFromSpace( float3 direction)
{
	return 1.0 - OW_Sky_SampleFromSpaceClouds( direction ).r;
}

float OW_SkyDotSunPlanePoint(in float3 aSunDirection, in float3 aViewDir, in float3 aPlaneIntersectionPoint, in float aPlaneIntersectionDistance)
{
	const float3 planeIntersectionDir = normalize(aPlaneIntersectionPoint);
	return aPlaneIntersectionDistance < 0? dot(aSunDirection, aViewDir) : dot(aSunDirection, planeIntersectionDir);
}

float OW_SkySmoothedNightDay(in float aDotSunPlanePoint, in float aShiftValue, in float aSmoothLength )
{
	return ((1.0f - step(aShiftValue, aDotSunPlanePoint)) * smoothstep(aShiftValue - aSmoothLength, aShiftValue, aDotSunPlanePoint)) + step(aShiftValue, aDotSunPlanePoint);
}

float4 OW_Sky_GetCloudsColorFromBelow(in float3 aViewOrigin, in float3 aViewDir)
{
	const float3 cameraPos = OW_Planet_GetCameraPos();
	const float3 sunDirection =	OW_Sun_GetPlanetToSunDir();

	float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 planeViewDir;
	float intersectionDistance;
	float3 intersectionPoint;
	OW_Sky_CloudsSamplingDir(aViewOrigin, aViewDir, planeViewDir, intersectionDistance, intersectionPoint);

	const float dotSunPlanePoint = OW_SkyDotSunPlanePoint(sunDirection, aViewDir, intersectionPoint, intersectionDistance);
	const float smoothedNightDay = OW_SkySmoothedNightDay(dotSunPlanePoint, 0.2f, 0.7f);

	color.rgb = GetAtmosphereColor(cameraPos, aViewDir, sunDirection);
	color.rgb = max( color.rgb, MR_OnworldEnvironmentConstants.onPlanetNightSkyColor );

@ifdef CLOUDS_ENABLED

	const float normalizedIntersectionDistance = OW_Sky_NormalizeIntersectionDistance(intersectionDistance);
	const float horizonBlend = normalizedIntersectionDistance * step(0.0f, intersectionDistance);

	// view direction
	const float3 uvw = OW_Sky_GetCloudUVWs(planeViewDir);
	const float4 c_neg = OW_Sky_SampleCloudCoverageNEG(uvw);
	const float4 c_pos = OW_Sky_SampleCloudCoveragePOS(uvw);

	// 6 Point Lighting
	const float planePointRadius = OW_Sky_GetCloudsPlanePointRadius();
	const float3 planeNormal = normalize(aViewOrigin);
	const float intersectionLength = length(intersectionPoint - planeNormal * planePointRadius);

	const float3 cameraForward = MR_GetZAxis(MR_ViewConstants.cameraToWorld).xyz;
	const float cloudsLODLerper = smoothstep(OW_PlanetConstants.const_CloudsLODFadeDistanceRange.x, OW_PlanetConstants.const_CloudsLODFadeDistanceRange.y, intersectionLength);
	const float cloudsHorizonLerper = lerp(OW_PlanetConstants.const_CloudsFade, 1.0f, cloudsLODLerper);
	const float c_density = lerp(0.0f, c_neg.a * horizonBlend, cloudsHorizonLerper);
	const float c_ambient = c_pos.a;

	const float3 sunDirectionNightDay = ((smoothedNightDay * 2.0f) - 1.0f) * sunDirection;

	float3 rotationAxis = cross(planeNormal, planeViewDir);
	const float normalSunSin = length(rotationAxis);
	rotationAxis = rotationAxis * rcp(normalSunSin);
	const float normalSunCos = dot(planeViewDir, planeNormal);

	const float3 rotatedSunDirection = MR_RotateAroundAxis(OW_Sky_CloudsFlowTransform(sunDirection), rotationAxis, normalSunCos, normalSunSin);
	float c_lighting =	saturate(rotatedSunDirection.x) * c_pos.r +
						saturate(rotatedSunDirection.y) * c_pos.g +
						saturate(rotatedSunDirection.z) * c_pos.b +
						saturate(-rotatedSunDirection.x) * c_neg.r +
						saturate(-rotatedSunDirection.y) * c_neg.g +
						saturate(-rotatedSunDirection.z) * c_neg.b;

	//Add Sky
	{
		const float ENV_SKY_AMBIENT_INTENSITY_MIN_cloudRayleighScale = 0.1f;
		const float ENV_SKY_AMBIENT_INTENSITY_MAX_cloudMieScale = 0.3f;
		const float c_rayleigh = lerp(1.0, lerp(ENV_SKY_AMBIENT_INTENSITY_MIN_cloudRayleighScale, ENV_SKY_AMBIENT_INTENSITY_MAX_cloudMieScale, c_ambient), c_density);
		color.rgb *= c_rayleigh;
	}

	// Add Clouds Direct Lighting
	{
		const float ENV_SUN_AMBIENT_INTENSITY_cloudUnlitScale = 0.05f;
		const float ENV_DIRECT_SUN_INTENSITY_cloudLitScale = 0.4f;
		const float3 sunColor = GetSunColorClamped( cameraPos, sunDirectionNightDay, 0.01f) * MR_OnworldEnvironmentConstants.onPlanetSunLightIntensity;
		const float3 moonColor = MR_OnworldEnvironmentConstants.onPlanetNightLightColorNoFactor;
		const float3 GFX_ENVIRONMENT_shadowLightAboveCloudsEmission = lerp(moonColor, sunColor, smoothedNightDay);
		color.rgb += GFX_ENVIRONMENT_shadowLightAboveCloudsEmission * lerp( c_ambient * ENV_SUN_AMBIENT_INTENSITY_cloudUnlitScale, ENV_DIRECT_SUN_INTENSITY_cloudLitScale, c_lighting ) * c_density;
	}

	//Add Clouds Rim Lighting
	{
		const float cosSV = dot( sunDirectionNightDay, -aViewDir );
		const float cos2SV = cosSV * cosSV;

		// extreme rim light near the sun direction
		const float miePhase2 = OW_Sky_GetMiePhase( cosSV, cos2SV, -0.8 ) + 0.05 * OW_Sky_GetMiePhase( cosSV, cos2SV, -0.97 );
		const float rimMask = c_density * (1.0f - c_density) + c_lighting;
		const float ENV_CLOUD_RIM_INTENSITY_cloudRimIntensity = 150.0f;
		const float3 atmosMie = GetAtmosphereMieColor(cameraPos, aViewDir, sunDirectionNightDay);
		color.rgb += rimMask * atmosMie * lerp(ENV_CLOUD_RIM_INTENSITY_cloudRimIntensity, ENV_CLOUD_RIM_INTENSITY_cloudRimIntensity * 0.4f, horizonBlend) * c_density * smoothedNightDay;
	}

	const float skyAlpha = saturate(1-c_density);
	color.a = skyAlpha;

@endif // CLOUDS_ENABLED

	return color;
}

float4 OW_Sky_GetCloudsColorFromBelow_CloudsParticles(in float3 aViewOrigin, in float3 aViewDir, out float3 anIntersectionPoint, out float anIntersectionDistance, out float aDensity)
{
	const float3 cameraPos = OW_Planet_GetCameraPos();
	const float3 sunDirection =	OW_Sun_GetPlanetToSunDir();

	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 planeViewDir;
	OW_Sky_CloudsSamplingDir(aViewOrigin, aViewDir, planeViewDir, anIntersectionDistance, anIntersectionPoint);

	const float dotSunPlanePoint = OW_SkyDotSunPlanePoint(sunDirection, aViewDir, anIntersectionPoint, anIntersectionDistance);
	const float smoothedNightDay = OW_SkySmoothedNightDay(dotSunPlanePoint, 0.2f, 0.7f);

	float3 atmosphereColor = max(MR_OnworldEnvironmentConstants.onPlanetNightSkyColor, GetAtmosphereColor(cameraPos, aViewDir, sunDirection));

	const float normalizedIntersectionDistance = OW_Sky_NormalizeIntersectionDistance(anIntersectionDistance);
	const float horizonBlend = normalizedIntersectionDistance * step(0.0f, anIntersectionDistance);

	// view direction
	const float3 uvw = OW_Sky_GetCloudUVWs(planeViewDir);
	const float4 c_neg = OW_Sky_SampleCloudCoverageNEGLod0(uvw);

	// 6 Point Lighting
	const float c_density = c_neg.a;

	const float3 sunDirectionNightDay = ((smoothedNightDay * 2.0f) - 1.0f) * sunDirection;

	//Add Sky
	color.rgb = atmosphereColor;

	// Add Clouds Direct Lighting
	{
		const float ENV_SUN_AMBIENT_INTENSITY_cloudUnlitScale = 0.05f;
		const float ENV_DIRECT_SUN_INTENSITY_cloudLitScale = 0.4f;
		const float3 sunColor = GetSunColorClamped( cameraPos, sunDirectionNightDay, 0.01f) * MR_OnworldEnvironmentConstants.onPlanetSunLightIntensity;
		const float3 moonColor = MR_OnworldEnvironmentConstants.onPlanetNightLightColorNoFactor;
		const float3 GFX_ENVIRONMENT_shadowLightAboveCloudsEmission = lerp(moonColor, sunColor, smoothedNightDay);
		color.rgb += GFX_ENVIRONMENT_shadowLightAboveCloudsEmission * max(0.1f, c_density);
	}

	color.a = lerp(0.0f, c_neg.a * horizonBlend, OW_PlanetConstants.const_CloudsFade);

	aDensity = c_neg.a;

	return color;
}