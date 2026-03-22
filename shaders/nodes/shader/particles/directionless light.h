<quarterSize = true, lightVolume = true>

import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/environment.h"
import "shaders/modules/graphics/shadows.h"
import "shaders/modules/graphics/lights.h"
import "shaders/modules/graphics/particles/spherical.h"
import "shaders/modules/graphics/lighting_data.h"
import "shaders/nodes/Shader/Functions/Lighting/GI.h"

struct DirectionlessLightParameters
{
	float3 myPos;
	float mySphericalShadingScale;
};

void DirectionlessLightCalculateLighting(inout float3 aLightInOut, Gfx_AttenuatedEmission aEmission,
	Gfx_LightingDataStruct aLD, DirectionlessLightParameters someParameters)
{
	float lightFactor = GetSelfShadowingFactor(someParameters.myPos, normalize(aEmission.diffuseToLight), someParameters.mySphericalShadingScale);
	aLightInOut += aEmission.diffuseEmission * lightFactor;
}

void DirectionlessLight(
	out float4 aLightOut,
	float4 aDiffuse,
	bool aUseShadows <default = true>,
	bool aUsePointLights <default = true>,
	float aSphericalShadingScale <default = 1>,
	float3 aPosition : Particle_Position,
	float4 Particle_ShaderParams : Particle_ShaderParams,
	float4 MR_FragCoord : MR_FragCoord
)
{
	if (aDiffuse.a <= 0.0)
	{
		discard;
		return;
	}

	ourIsTransparent = true;

	aLightOut = float4(0,0,0,1);

	float zdistance = Particle_ShaderParams.x;
	float3 pos = MR_ClipToWorld(MR_PixelToClip(MR_FragCoord.xy), zdistance);

@ifndef GFX_IS_PARTICLE_SHADOW
	float3 giNormal = normalize( pos - ourLightSphere.xyz );

	Gfx_LightingDataStruct ld = Gfx_GetLightingDataDummy(pos, MR_FragCoord.xy, MR_FragCoord.w, Particle_IsIndoor());

	bool useFarFog = true;

@ifdef GFX_USE_LIGHT_VOLUME
	useFarFog = false;
//	aLightOut.xyz += SampleLightVolume(pos);
@else
	{
	@ifdef GFX_FILL_LIGHT_GI
		aLightOut.rgb += CalculateFillLightGI( giNormal, float3(1,1,1), float3(0,0,0) );
	@else
		aLightOut.rgb += MR_ParticleGISamples[ourGISampleIndex][0].xyz;
// 		aLightOut.rgb += SampleGI( giNormal, ld );
	@endif
	
		float3 toSunDir = MR_ParticleGISamples[ourGISampleIndex][1].xyz;
		float3 sunColor = MR_ParticleGISamples[ourGISampleIndex][2].xyz;
		float3 sunLight = sunColor;
		if (aUseShadows)
		{
	@ifdef GFX_FILL_LIGHT_GI
			float3 shadowFactor = GetShadowFactor(pos, MR_FragCoord.xy);
			sunLight = lerp( Gfx_Environment.FillLightGIShadowColor, sunLight, shadowFactor );
	@else
			sunLight *= GetShadowFactor(pos, MR_FragCoord.xy);
	@endif
			sunLight *= GetSelfShadowingFactor(pos, toSunDir, aSphericalShadingScale);
		}
		aLightOut.rgb += sunLight;

		if (aUsePointLights)
		{
			DirectionlessLightParameters params;
			params.myPos = pos;
			params.mySphericalShadingScale = aSphericalShadingScale;
	
	@include "shaders/modules/graphics/lights_code.h"
			CalculateLighting2(DirectionlessLightCalculateLighting, aLightOut.rgb, ld, params);
		}
	}
@endif

	aLightOut *= aDiffuse;

@ifndef GFX_IS_PARTICLE_SHADOW
	const float4 fog = MR_ParticleGISamples[ourGISampleIndex][3];
	OW_Fog_ApplyExtinction( aLightOut.rgb, fog );
	@ifdef OGFX_IS_BLENDED
		OW_Fog_AddFogColor( aLightOut.rgb, fog );
	@endif // OGFX_IS_BLENDED
@endif
@ifdef OGFX_IS_BACKDROP // ONWARD: Don't use this for backdrop
	aLightOut = float4(1,0,1,1);
@endif // OGFX_IS_BACKDROP
}
