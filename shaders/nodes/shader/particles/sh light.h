import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/environment.h"
import "shaders/modules/graphics/shadows.h"
import "shaders/modules/graphics/lights.h"
import "shaders/modules/graphics/particles/constants.h"
import "shaders/nodes/Shader/Functions/Lighting/GI.h"

void GetParticleSH(float4 aPackedSH, float2 aInstanceUV, out float4 aSHOut, out float aAlphaOut)
{
	float2 offset = (aInstanceUV - vec2(0.5)) * float2(2,-2);
	float offsetlen = length(offset);
	float mask = saturate(1.0 - offsetlen);
	if (mask <= 0.0)
		discard;

	float offsetlen2 = offsetlen * offsetlen;
	float2 offset2 = normalize(offset) * offsetlen2;
	float mask2 = 1.0 - offsetlen2;

	float edgeMask = saturate(1.0 - 3.0 * abs(0.2 - mask));

	float4 sh = vec4(0);
@ifndef GFX_IS_PARTICLE_SHADOW
	float4 originalSH;
	originalSH.xyz = aPackedSH.xyz * 2.0 - vec4(1);
	originalSH.w = aPackedSH.w;

	sh = originalSH;
	sh.xyz = normalize(sh.xyz);
	sh.xy += offset2 * 0.2;
	sh.z -= edgeMask * 0.9;
// 	sh.z += (mask2 - 0.5) * 0.1;
	sh.z *= 0.3;
	sh.xyz = normalize(sh.xyz) * 0.5;
	sh.w *= 0.5;

	float4 maxSH = max(originalSH, -originalSH);
	float2 max2 = max(maxSH.xy, maxSH.zz);
	float max1 = max(max2.x, max2.y);
	aAlphaOut = lerp(0.0, mask2, max1);
@else
	aAlphaOut = mask2 * 0.25;
@endif

	aSHOut = sh;
}

float3 SHLightCalculateLighting(float3 aLightEmission, float3 aToLight, float4 aSH)
{
	float lightFactor = saturate(aSH.w + dot(aSH.xyz, normalize(aToLight)));
	return aLightEmission * lightFactor;
}

void SHLight(
	out float4 aLightOut,
	float4 aPackedSH,
	float4 aDiffuse,
	bool aUseShadows <default = true>,
	bool aUsePointLights <default = true>,
	float2 aInstanceUV : Particle_InstanceUV,
	float3 aPosition : Particle_Position,
	float4 Particle_ShaderParams : Particle_ShaderParams,
	float4 MR_FragCoord : MR_FragCoord
)
{
	ourIsTransparent = true;
	
	aLightOut = vec4(0);

	float zdistance = Particle_ShaderParams.x;
	float3 pos = MR_ClipToWorld(MR_PixelToClip(MR_FragCoord.xy), zdistance);

	float4 sh;
	GetParticleSH(aPackedSH, aInstanceUV, sh, aLightOut.a);

@ifndef GFX_IS_PARTICLE_SHADOW
	sh.xyz = MR_TransformNormal(MR_ViewConstants.cameraToWorld, sh.xyz * float3(1,1,-1));

	aLightOut.rgb = MR_ParticleGISamples[ourGISampleIndex][0].xyz;
//	aLightOut.rgb = SampleGI( sh, pos );

	Gfx_DirectionalLight sun = Gfx_GetShadowLight();
	float sunFactor = saturate(sh.w + dot(sh.xyz, sun.toLightDirection));
	float3 sunLight = sun.emission * sunFactor;
	if (aUseShadows)
		sunLight *= GetShadowFactor(pos, MR_FragCoord.xy);
	aLightOut.rgb += sunLight;

	if (aUsePointLights)
	{
@include "shaders/modules/graphics/lights_code.h"
		CalculateLighting(SHLightCalculateLighting, aLightOut.rgb, pos, MR_FragCoord.xy, MR_FragCoord.w, 1.0, sh);
	}
@endif

	aLightOut *= aDiffuse;

@ifndef GFX_IS_PARTICLE_SHADOW
	aLightOut.rgb = Gfx_Fog2(aLightOut.rgb, -pos, Particle_ShaderParams.x);
@endif
}
