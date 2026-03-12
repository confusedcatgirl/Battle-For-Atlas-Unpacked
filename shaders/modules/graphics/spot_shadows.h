@ifndef GFX_NO_SHADOWS
@ifndef GFX_DEBUG_NO_SPOTLIGHT_SHADOW
@ifdef GFX_SPOT_SHADOWS
	@define SPOT_SHADOWS_ENABLED
@endif
@endif
@endif

import "shaders/modules/graphics/lights_common.h"
import "shaders/modules/graphics/lights.h"

@ifdef SPOT_SHADOWS_ENABLED

import "shaders/modules/graphics/shadow_functions.h"
import "shaders/modules/graphics/shadows_common.h"

	MR_Sampler2DArrayCmp SpotlightShadowMap : MR_SpotlightShadowMap
	{
		filter = linear point,
		wrap = clamp,
		compare = >
	};

	constants SpotlightShadowConstants : MR_SpotlightShadowConstants
	{
		float shadowMapOffset;
		float3 unused;
		float4x4 worldToShadow[17];
	}

	float3 GetSpotlightShadowFactor(Gfx_Light aLight, float3 aWorldSpacePos, float2 aFragCoord)
	{
		if (aLight.shadowMapIndex < 0)
			return vec3(1.0);

		float4 shadowPos = MR_Transform(SpotlightShadowConstants.worldToShadow[aLight.shadowMapIndex], aWorldSpacePos);
		shadowPos.xyz /= shadowPos.w;

		float4 uv = float4(shadowPos.xy * float2(0.5,-0.5) + vec2(0.5), aLight.shadowMapIndex, shadowPos.z);
		float2 rotateVec = GetRotateVec(aFragCoord);
		float3 shadowFactor = vec3(CalcShadowFactorArray(SpotlightShadowMap, uv, rotateVec, SpotlightShadowConstants.shadowMapOffset));

		return shadowFactor;
	}

@else

	float3 GetSpotlightShadowFactor(Gfx_Light aLight, float3 aWorldSpacePos, float2 aFragCoord)
	{
		return vec3(1.0);
	}

@endif // SPOT_SHADOWS_ENABLED

bool Gfx_GetSpotShadowAttenuatedEmission(out Gfx_AttenuatedEmission aEmissionOut, Gfx_Light aLight, float3 aWorldSpacePos, float2 aFragCoord)
{
	if (!Gfx_GetAttenuatedEmission(aEmissionOut, aLight, aWorldSpacePos))
		return false;

@ifdef SPOT_SHADOWS_ENABLED
	float3 shadowFactor = GetSpotlightShadowFactor(aLight, aWorldSpacePos, aFragCoord);

	shadowFactor = lerp(vec3(1.0), shadowFactor, aLight.influence);

	if (shadowFactor.x <= 0)
		return false;

	aEmissionOut.diffuseEmission *= shadowFactor;
	aEmissionOut.specEmission *= shadowFactor;
@endif

	return true;
}

@ifdef GFX_DEFERRED
@ifdef GFX_SS_SHADOW_SPOT_LIGHTS
import "shaders/modules/graphics/screenspace_shadows.h"
@endif
@endif

bool Gfx_GetSpotShadowAttenuatedEmissionIdeal(out Gfx_AttenuatedEmission aEmissionOut, Gfx_Light aLight, Gfx_LightingDataStruct aLightingData, float3 aNormal)
{
	if (!Gfx_GetAttenuatedEmissionIdeal(aEmissionOut, aLight, aLightingData, aNormal))
		return false;

@ifdef SPOT_SHADOWS_ENABLED
	float3 shadowFactor = GetSpotlightShadowFactor(aLight, aLightingData.worldPosition, aLightingData.pixelPosition);

@ifdef GFX_DEFERRED
@ifdef GFX_SS_SHADOW_SPOT_LIGHTS
	if (aLight.shadowMapIndex >= 0)
	{
		float3 toLight = aLight.position - aLightingData.worldPosition;
		SSSh_FriendlySpotShadow(shadowFactor, toLight, aLightingData);
	}
@endif
@endif

	shadowFactor = lerp(vec3(1), shadowFactor, aLight.influence);

	if (shadowFactor.x <= 0)
		return false;

	aEmissionOut.diffuseEmission *= shadowFactor;
	aEmissionOut.specEmission *= shadowFactor;
@endif

	return true;
}

bool Gfx_GetSpotShadowAttenuatedEmission(out Gfx_AttenuatedEmission aEmissionOut, Gfx_Light aLight, Gfx_LightingDataStruct aLightingData)
{
	if (!Gfx_GetAttenuatedEmission(aEmissionOut, aLight, aLightingData))
		return false;

@ifdef SPOT_SHADOWS_ENABLED
	float3 shadowFactor = GetSpotlightShadowFactor(aLight, aLightingData.worldPosition, aLightingData.pixelPosition);

	shadowFactor = lerp(vec3(1), shadowFactor, aLight.influence);

	if (shadowFactor.x <= 0)
		return false;

	aEmissionOut.diffuseEmission *= shadowFactor;
	aEmissionOut.specEmission *= shadowFactor;
@endif

	return true;
}
