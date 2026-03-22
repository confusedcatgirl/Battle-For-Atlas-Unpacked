import "shaders/modules/graphics/gpuparticles/gpuparticles_common.h"
import "shaders/nodes/Shader/Functions/Onward/OW_GetTerrainLowResRadius.h"
import "shaders/modules/graphics/onworld/atmosphere/planet_sky.h"
import "shaders/math/intersect.h"

float CameraFrustum_Intersect_Always_Pos(float3 aOrigin, float3 aDirection)
{
	float3 intersectionPoint = 0.0f;
	float minT = INTERSECT_FLT_MAX;
	float t = Ray_PlaneIntersect(MR_ViewConstants.leftPlane, aOrigin, aDirection);
	minT = t > 0.0f && t < minT? t : minT;
	t = Ray_PlaneIntersect(MR_ViewConstants.rightPlane, aOrigin, aDirection);
	minT = t > 0.0f && t < minT? t : minT;
	t = Ray_PlaneIntersect(MR_ViewConstants.topPlane, aOrigin, aDirection);
	minT = t > 0.0f && t < minT? t : minT;
	t = Ray_PlaneIntersect(MR_ViewConstants.bottomPlane, aOrigin, aDirection);
	minT = t > 0.0f && t < minT? t : minT;

	return minT;
}

bool IsParticleFrustumCulled(float3 aPositionWorldSpace)
{
@ifdef SPACE_2_GROUND
	const float threshold = -140.0f;
@else
	const float threshold = -200.0f;
@endif
	return	(DistanceToPlane(MR_ViewConstants.rightPlane, aPositionWorldSpace) < threshold)		||
			(DistanceToPlane(MR_ViewConstants.leftPlane, aPositionWorldSpace) < threshold)		||
			(DistanceToPlane(MR_ViewConstants.topPlane, aPositionWorldSpace) < threshold)		||
			(DistanceToPlane(MR_ViewConstants.bottomPlane, aPositionWorldSpace) < threshold);
}

bool IsParticleCulledInCloudsPlane(float3 aParticlePositionPlanetSpace, float3 aPlanePositionPlanetSpace)
{
	return length(aParticlePositionPlanetSpace - aPlanePositionPlanetSpace) > mySpawnCullDistance;
}

bool IsParticleDead(float3 aParticlePositionPlanetSpace)
{
@ifdef SPACE_2_GROUND
	return length(aParticlePositionPlanetSpace) > OW_Planet_GetCameraRadius();
@else
	return false;
	// const float3 cameraForward = MR_GetZAxis(MR_ViewConstants.cameraToWorld).xyz;
	// const float3 particleDirFromCam = aParticlePositionPlanetSpace - OW_Planet_GetCameraPos();
	// return dot(particleDirFromCam, cameraForward) <= 0.0f;// || length(particleDirFromCam) < OW_Planet_GetCameraRadius();
@endif
}

void UpdateParticles(in int aParticleIndex, in MR_Sampler2D<float> aDepthBuffer, inout SParticleData aParticleData, out SOutputVertices someOutputVertices)
{
	const float3 cameraRight = MR_GetXAxis(MR_ViewConstants.cameraToWorld).xyz;
	const float3 cloudsPlaneNormal = normalize(OW_Planet_GetCameraPos());
	const float3 cameraForward = MR_GetZAxis(MR_ViewConstants.cameraToWorld).xyz;

@ifdef SPACE_2_GROUND
	const float atmosphereRadius = OW_Planet_GetAtmosphereRadius();
	const float playgroundRadius = OW_Planet_GetMinTransitionRegionRadius();
	const float cameraDistance = OW_Planet_GetCameraRadius();
	const float transitionFactor = 1.0f - min(((cameraDistance - playgroundRadius) / (atmosphereRadius - playgroundRadius)), 1.0f);
	const float cloudsPlanePointRadius = lerp(Atm_GetSmoothedRadius(), atmosphereRadius, transitionFactor);

	const float3 transformedCloudsPlaneNormal = OW_Sky_CloudsFlowTransformForce(cloudsPlaneNormal, 1.0f);
	const float3 cloudsDirection = normalize(transformedCloudsPlaneNormal - cloudsPlaneNormal);
@else
	const float atmosphereRadius = OW_Planet_GetAtmosphereRadius();
	const float playgroundRadius = OW_Planet_GetMinTransitionRegionRadius();
	const float cameraDistance = OW_Planet_GetCameraRadius();
	const float transitionFactor = saturate(((cameraDistance - playgroundRadius) / (atmosphereRadius - playgroundRadius)));

	const float cloudsPlanePointRadius = OW_Sky_GetCloudsPlanePointRadius();
@endif

	uint randomQ[4];
	SetRandomSeed(aParticleIndex,randomQ);
	Random01(randomQ); Random01(randomQ);

	if (aParticleData.myFlags & 32)
	{
		aParticleData.myFlags = Random01(randomQ) * 8;
@ifdef SPACE_2_GROUND
		const float3 rayOrigin =  cloudsPlaneNormal * cloudsPlanePointRadius + normalize(cross(cloudsPlaneNormal, cloudsDirection)) * (cameraDistance - cloudsPlanePointRadius) * (Random01(randomQ) * 2.0f - 1.0f);
		const float t = CameraFrustum_Intersect_Always_Pos(rayOrigin + OW_Planet_GetPosition(), cloudsDirection);
		const float3 positionEdgesPlanetSpace = rayOrigin + cloudsDirection * t * (Random01(randomQ) * 0.9f + 0.1f);

		const float3 randomPositionPlanetSpace = positionEdgesPlanetSpace;

		aParticleData.myFlags |= 16;

		aParticleData.myPos = normalize(randomPositionPlanetSpace);
		aParticleData.myStartTime = myCurrentTime;
		aParticleData.myFlags = aParticleData.myFlags | (RandomUInt(randomQ) << 6);
		aParticleData.myMass = (Random01(randomQ) * 2.0f - 1.0f);

		aParticleData.myVelocity.x = OW_Sky_GetCloudsDensityFromSpaceLod0(aParticleData.myPos);
@else
		const float3 forward = normalize(cameraForward - dot(cameraForward, cloudsPlaneNormal) * cloudsPlaneNormal);
		const float3 right = normalize(cross(cloudsPlaneNormal, forward));

@ifdef GROUND_2_SPACE
		// const float3 randomPositionPlanetSpace =	normalize(OW_Planet_GetCameraPos()) * cloudsPlanePointRadius +
		// 											forward * OW_PlanetConstants.const_CloudsParticlesSpawningRadius * Random01(randomQ) * 0.5f +
		// 											right * (Random01(randomQ) * 2.0f - 1.0f) * OW_PlanetConstants.const_CloudsParticlesSpawningRadius * 0.5f;

		const float2 pixelPosition = MR_FramebufferUVToPixel(float2(Random01(randomQ), Random01(randomQ)));
		const float2 clipPosition = MR_PixelToClip(pixelPosition);
		const float3 randomPositionPlanetSpace = MR_ClipToWorld(clipPosition, 2000.0f) - OW_Planet_GetPosition();
@else
		const float3 randomPositionPlanetSpace =	normalize(OW_Planet_GetCameraPos()) * cloudsPlanePointRadius +
													forward * OW_PlanetConstants.const_CloudsParticlesSpawningRadius * Random01(randomQ) +
													right * (Random01(randomQ) * 2.0f - 1.0f) * OW_PlanetConstants.const_CloudsParticlesSpawningRadius;
@endif

		aParticleData.myFlags |= 16;

		aParticleData.myPos = normalize(randomPositionPlanetSpace);
		aParticleData.myStartTime = myCurrentTime;
		aParticleData.myFlags = aParticleData.myFlags | (RandomUInt(randomQ) << 6);
		aParticleData.myMass = (Random01(randomQ) * 2.0f - 1.0f);
		aParticleData.myVelocity.xy = float2(Random01(randomQ), Random01(randomQ));
		aParticleData.myVelocity.z = 0.0f;

		// @ifdef GROUND_2_SPACE
		// 	aParticleData.myVelocity.z = 500.0f * (Random01(randomQ) + 1.0f) * 0.5f;
		// @endif
@endif
	}
	
	const float startTime = aParticleData.myStartTime;
	const float age = myCurrentTime - startTime;

@ifdef SPACE_2_GROUND
	float3 particleDirectionPlanetSpace = OW_Sky_CloudsFlowTransform(aParticleData.myPos, -age);
	float3 particlePositionPlanetSpace = particleDirectionPlanetSpace * lerp(cloudsPlanePointRadius, cloudsPlanePointRadius + OW_PlanetConstants.const_CloudsParticlesSpawningHeightOffset * aParticleData.myMass, transitionFactor);
	const float alpha = OW_PlanetConstants.const_CloudsParticlesFade * OW_PlanetConstants.const_CloudsParticlesAlphaStrength * smoothstep(0, 1.0f, age);
	float4 color = 0.0f;
	color.rgb = OW_Sky_GetCloudsColorFromSpace_Particles(particlePositionPlanetSpace + OW_Planet_GetPosition());

	if (
		aParticleData.myVelocity.x < OW_PlanetConstants.const_CloudsParticlesDensityCulling
		||
		IsParticleFrustumCulled(particlePositionPlanetSpace + OW_Planet_GetPosition())
		||
		IsParticleDead(particlePositionPlanetSpace)
		)
	{
		aParticleData.myFlags |= 32;
		aParticleData.myFlags |= 16;
		aParticleData.myFlags = aParticleData.myFlags | (RandomUInt(randomQ) << 6);
	}
@else
	float3 particlePositionPlanetSpace;

	const float3 viewOrigin = OW_Planet_GetCameraPos();

	const float3 rayDir = OW_Sky_CloudsFlowTransform(aParticleData.myPos, -age);
	const float3 planeNormal = normalize(viewOrigin);
	float intersectionDistance = Plane_RayIntersect(0, rayDir, planeNormal, planeNormal * cloudsPlanePointRadius);

	float3 viewDir = normalize(rayDir * intersectionDistance - viewOrigin);
	float density = 0.0f;
	float4 color =  OW_Sky_GetCloudsColorFromBelow_CloudsParticles(viewOrigin, viewDir, particlePositionPlanetSpace, intersectionDistance, density);

	@ifdef GROUND_2_SPACE
		particlePositionPlanetSpace +=  planeNormal * aParticleData.myVelocity.z;
	@endif
	if (
		density <= OW_PlanetConstants.const_CloudsParticlesDensityCulling 
		|| IsParticleCulledInCloudsPlane(particlePositionPlanetSpace, planeNormal * cloudsPlanePointRadius) 
@ifdef GROUND_2_SPACE
		|| IsParticleFrustumCulled(particlePositionPlanetSpace + OW_Planet_GetPosition())
@endif
		|| IsParticleDead(particlePositionPlanetSpace)
		)
	{
		aParticleData.myFlags |= 32;
		aParticleData.myFlags |= 16;
		aParticleData.myFlags = aParticleData.myFlags | (RandomUInt(randomQ) << 6);
	}

	const float alpha = OW_PlanetConstants.const_CloudsParticlesFade * lerp(OW_PlanetConstants.const_CloudsParticlesAlphaStrength, 0.0f, saturate(1.0f - age)) * (((mySpawnCullDistance - intersectionDistance) / mySpawnCullDistance));
@endif

	const float3 planet_pos = OW_Planet_GetPosition();
	float3 positionCameraSpace = MR_Transform(MR_ViewConstants.unjitteredWorldToCamera, particlePositionPlanetSpace + planet_pos).xyz;
	if (aParticleData.myFlags & 32)
		positionCameraSpace.z = -100.0f;

@ifdef SPACE_2_GROUND
	// TODO this is good enough for now
	const float3 directionCameraSpace = 0.0f;
@else
	// Project right camera vector onto clouds plane
	const float3 rotatedCloudsPlaneNormal = normalize(OW_Planet_GetCameraPos() + aParticleData.myVelocity.x * cameraRight * 5000.0f + aParticleData.myVelocity.y * 5000.0f * cameraForward);
	const float3 projCameraRightOntoCloudsPlane = normalize(cameraRight - dot(rotatedCloudsPlaneNormal, cameraRight) * rotatedCloudsPlaneNormal);
	const float3 directionCameraSpace = MR_Transform(MR_ViewConstants.unjitteredWorldToCamera, float4(projCameraRightOntoCloudsPlane, 0.0f)).xyz;
@endif

	const uint textureIdx = 0;

	someOutputVertices.myCameraPos = positionCameraSpace;
	someOutputVertices.myTextureIdx = textureIdx;
	someOutputVertices.myLighting = color.rgb * myTintColor;
	someOutputVertices.myAlpha = alpha;
	someOutputVertices.myCameraDir = directionCameraSpace;
	someOutputVertices.mySize = mySize;
}
