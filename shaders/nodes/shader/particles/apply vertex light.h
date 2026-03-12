<use4x4Tesselation = true, lightVolume = true, vertexLighting = F"shaders/nodes/Shader/Particles/Apply Vertex Light.h">

import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/environment.h"
import "shaders/modules/graphics/shadows.h"
import "shaders/modules/graphics/lights.h"
import "shaders/modules/graphics/lighting_data.h"
import "shaders/modules/graphics/particles/constants.h"
import "shaders/nodes/Shader/Functions/Lighting/GI.h"
import "shaders/modules/graphics/particles/sample.h"

struct VertexLightParameters
{
};

void VertexCalculateLighting(inout float3 aLightInOut, Gfx_AttenuatedEmission aEmission,
	Gfx_LightingDataStruct aLD, VertexLightParameters someParameters)
{
	aLightInOut += aEmission.diffuseEmission;
}

void VertexLight(
	out float4 aLightOut,
	out float4 aFogOut,
	float3 aFlattenedWorldPos,
	float3 anUnflattenedWorldPos,
	float4 aClipPos,
	float4 aColor
	)
{
	aLightOut = aColor;
	aFogOut = vec4(0);
 
	float3 ambient;
	float3 sunDir, sunColor;

	const float3 upVector = normalize( anUnflattenedWorldPos - OW_Planet_GetBaseGroundSphere( ).xyz );

	float2 clip2 = aClipPos.xy / aClipPos.w;
	clip2 = clamp(clip2, vec2(-0.99), vec2(0.99));
	float2 fc = MR_ClipToPixel(clip2);

	OW_SampleParticles( anUnflattenedWorldPos, aFlattenedWorldPos, upVector, fc, ambient, sunDir, sunColor, aFogOut );

@ifndef GFX_IS_PARTICLE_SHADOW

	float3 shadow = GetShadowFactor(aFlattenedWorldPos, vec2(0));

	Gfx_LightingDataStruct ld = Gfx_GetLightingDataDummy(aFlattenedWorldPos, fc, aClipPos.w, Particle_IsIndoor());

	//bool useFarFog = true;
	//if (Particle_UseLightVolume())
	//{
	//	useFarFog = false;
	//	aLightOut.xyz *= SampleLightVolume(worldPos);
	//}
	//else
	{
@include "shaders/modules/graphics/lights_code.h"
		VertexLightParameters params;
		float3 pointLights = vec3(0);
		CalculateLighting2(VertexCalculateLighting, pointLights, ld, params);
		aLightOut.xyz *= ambient + sunColor * shadow + pointLights;
	}
@endif // GFX_IS_PARTICLE_SHADOW
}

void VertexLight(
	out float4 aLightOut,
	out float4 aFogOut,
	float4 aClipPos,
	float4 aColor
)
{
	float4 worldPos4 = MR_Transform(MR_ViewConstants.clipToWorld, aClipPos);
	float3 flattenedWorldPos = worldPos4.xyz / worldPos4.w;
	float3 unflattenedWorldPos = flattenedWorldPos;
	// OW_SFT_ApplyUnflattening( unflattenedWorldPos ); // TODO need unflattened position
	VertexLight( aLightOut, aFogOut, flattenedWorldPos, unflattenedWorldPos, aClipPos, aColor );
}

void ApplyVertexLight(
	out float4 aLightOut,
	float4 aColor,
	float4 aFog
)
{
@ifndef GFX_IS_PARTICLE_SHADOW
	aLightOut = aColor;
	OW_Fog_ApplyExtinction( aLightOut.rgb, aFog );
	@ifdef OGFX_IS_BLENDED
		OW_Fog_AddFogColor( aLightOut.rgb, aFog );
	@endif // OGFX_IS_BLENDED
@else
	aLightOut = float4(0,0,0, aColor.w);
@endif // GFX_IS_PARTICLE_SHADOW
}
