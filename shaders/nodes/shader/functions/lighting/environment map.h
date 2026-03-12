import "shaders/modules/graphics/environment.h"
import "shaders/modules/graphics/lights.h"
import "shaders/nodes/Shader/Functions/Lighting/GI.h"

MR_SamplerCube EnvironmentMap : MR_EnvironmentMap0 { anisotropy = 0 };
MR_SamplerCube DistantEnvironmentMap : MR_EnvironmentMap1 { anisotropy = 0 };
MR_Sampler2D EnvironmentMapBRDF : MR_EnvironmentMapBRDF { wrap = clamp };

float3 SampleEnvMap(float3 aDirection, float aRoughnessModified)
{
	float lodFactor = Gfx_Environment.environmentMapLodFactor;
	return MR_SampleLod(EnvironmentMap, aDirection, aRoughnessModified * lodFactor).xyz;
}

float3 SampleEnvMap(float3 aDirection, float aRoughnessModified, Gfx_LightingDataStruct lightingData : Gfx_LightingData, bool Gfx_IsIndoor : Gfx_IsIndoor)
{
	float lodFactor = Gfx_Environment.environmentMapLodFactor;
	float shRange = 2.0 / lodFactor;
	float envMapRoughness = aRoughnessModified * (1 + shRange);

	float3 shLight = SampleGI( aDirection, lightingData );

	float3 envMapLight = vec3(0);
//     if (aRoughnessModified < 0.999) // disabled because of compiler bug
	{
		float lod = aRoughnessModified * lodFactor;

@ifndef GFX_DEFERRED
		{
			// restrict lod to reduce aliasing
			float3 ndir = normalize(aDirection);
			float3 ndirdx = ddx(ndir);
			float3 ndirdy = ddy(ndir);
			float3 ndird = ndirdx + ndirdy;
			float delta = length(ndird);
			float minlod = log2(delta * 10);
			lod = max(minlod, lod);
		}
@endif

		float3 lastLod;

@ifndef GFX_NO_INDOOR
		/* if (Gfx_IsIndoor)
		{
			envMapLight = MR_SampleLod(IndoorEnvironmentMap, aDirection, lod).xyz;
			lastLod = MR_SampleLod(IndoorEnvironmentMap, aDirection, 100).xyz;
		}
		else */
@endif // GFX_NO_INDOOR
		{
			envMapLight = MR_SampleLod(EnvironmentMap, aDirection, lod).xyz;
			lastLod = MR_SampleLod(EnvironmentMap, aDirection, 100).xyz;

@ifndef GFX_NO_DISTANT_ENVMAP
			{
				float direction_t = saturate(aDirection.y);
				float distanceScale = 1.0 / (1.1 - direction_t);
				float distance_t = (distanceScale * lightingData.depth - 100.0) / 500.0;
				float t = saturate(distance_t * saturate(direction_t * 3));

				if (t > 0)
				{
					envMapLight = lerp(envMapLight, MR_SampleLod(DistantEnvironmentMap, aDirection, lod).xyz, t);
					lastLod = lerp(lastLod, MR_SampleLod(DistantEnvironmentMap, aDirection, 100).xyz, t);
				}
			}
@endif
		}

		float3 normalized = (envMapLight * shLight) / max(0.0001, lastLod);

		envMapLight = lerp(envMapLight, normalized, saturate(aRoughnessModified * 4));

		envMapLight = envMapLight * Gfx_Environment.environmentMapFactor;
	}

	return lerp(envMapLight, shLight, saturate(envMapRoughness - 1.0) / shRange);
}

float OW_EnvMap_GetLODFromRoughness(float3 aDirection, float aRoughness, in float aLODFactor)
{
	float lod = aRoughness * aLODFactor;

@ifndef GFX_DEFERRED // restrict lod to reduce aliasing (taken from Division)
	{
		float3 ndir = normalize(aDirection);

		float3 ndirdx = ddx(ndir);
		float3 ndirdy = ddy(ndir);

		float3 ndird = ndirdx + ndirdy;

		float delta = length(ndird);
		float minlod = log2(delta * 10);
		lod = max(minlod, lod);
	}
@endif // GFX_DEFERRED

	return lod;
}

float3 OW_EnvMap_GetNormalizedSample_2(MR_SamplerCube aSampler, in float3 aDirection, in float aRoughness, in float3 aNormalizationColor, in float aLODFactor)
{
	float3 envMapLight = vec3(0);

	{
		const float lod = OW_EnvMap_GetLODFromRoughness(aDirection, aRoughness, aLODFactor);
		envMapLight = MR_SampleLod(aSampler, aDirection, lod).xyz;

		const float3 roughest = MR_SampleLod(aSampler, aDirection, 100.0).xyz;
		float3 normalized = (envMapLight * aNormalizationColor) / max(0.0001, roughest);

		envMapLight = lerp(envMapLight, normalized, saturate(aRoughness * 4.0));
	}

	const float normalizationRange = 2.0 / aLODFactor;
	const float envRoughness = aRoughness * (1.0 + normalizationRange);
	return lerp(envMapLight, aNormalizationColor, saturate(envRoughness - 1.0) / normalizationRange);
}

float3 OW_EnvMap_GetNormalizedSample_1(in float3 aDirection, in float aRoughness, in float3 aNormalizationColor)
{
	return OW_EnvMap_GetNormalizedSample_2(EnvironmentMap, aDirection, aRoughness, aNormalizationColor, Gfx_Environment.environmentMapLodFactor);
}

