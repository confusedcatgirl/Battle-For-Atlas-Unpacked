<vertexLighting = F"shaders/nodes/Shader/Particles/Apply Vertex Ambient.h">

import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/environment.h"
import "shaders/modules/graphics/shadows.h"
import "shaders/modules/graphics/lights.h"
import "shaders/modules/graphics/lighting_data.h"
import "shaders/modules/graphics/particles/constants.h"
import "shaders/nodes/Shader/Functions/Lighting/GI.h"

void VertexLight(
	out float4 aLightOut,
	out float4 aFogOut,
	float4 aClipPos,
	float4 aColor
)
{
	aLightOut = aColor;
	aFogOut = vec4(0);

@ifndef GFX_IS_PARTICLE_SHADOW
	float4 worldPos4 = MR_Transform(MR_ViewConstants.clipToWorld, aClipPos);
	float3 worldPos = worldPos4.xyz / worldPos4.w;

    float3 ambient = SampleGI( float3(0,1,0), worldPos );
	aLightOut.xyz *= ambient;

	float2 clip2 = aClipPos.xy / aClipPos.w;
	clip2 = clamp(clip2, vec2(-0.99), vec2(0.99));
	float2 fc = MR_ClipToPixel(clip2);

	Gfx_LightingDataStruct ld = Gfx_GetLightingDataDummy(worldPos, fc, aClipPos.w, Particle_IsIndoor());

	float3 vertexToCamera = MR_GetPos(MR_ViewConstants.cameraToWorld).xyz - worldPos;
	float3 fogResult0 = Gfx_Fog3(vec3(0), vertexToCamera, length(vertexToCamera), true, ld, true, clip2, !ld.isIndoor);
	float3 fogResult1 = Gfx_Fog3(vec3(1), vertexToCamera, length(vertexToCamera), true, ld, true, clip2, !ld.isIndoor);

	float diff = fogResult1.x - fogResult0.x;
	float alpha = 1.0 - diff;

	aFogOut.w = alpha;
	aFogOut.xyz = fogResult0 / max(alpha, 0.000001);
@endif
}

void ApplyVertexAmbient(
	out float4 aLightOut,
	float4 aColor,
	float4 aFog,
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
@endif
}
