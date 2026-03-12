import "shaders/modules/graphics/onward/gi/fog/fog_shared.h"
import "shaders/modules/graphics/onward/gi/fog/fog_tiles.h"

MR_RWTexture3D<float4> IntegratedVolumeRW : MR_RW_Texture0;
MR_RWTexture2D<float4> IntegratedLookupRW : MR_RW_Texture1;

@ifdef OGI_FOG_REPROJECT
MR_Texture3D<float4> LastFrameIntegration : MR_Texture0;
MR_Sampler3D<float4> LastFrameIntegrationSampler : MR_Texture1 { filter = linear point, wrap = clamp, anisotropy = 0 };
@endif // OGI_FOG_REPROJECT

MR_Texture3D<float3> LitMediaTexture : MR_Texture2;
MR_Sampler3D<float3> LitMediaTextureSampler : MR_Texture3 { filter = linear point, wrap = clamp, anisotropy = 0 };
MR_Texture3D<float2> PropertyTexture : MR_Texture4;
MR_Sampler3D<float2> PropertyTextureSampler : MR_Texture5 { filter = linear point, wrap = clamp, anisotropy = 0 };

constants : MR_ConstantBuffer8
{
	float3 constJitter;
	uint constTileToThread;

	uint3  constIntegrationSize;
	float3 constIntegrationInvSize;
}

float4 OW_FogIntegration_GetOptical( in float3 aColor, in float2 aScatteringAbsorption )
{
	return float4( aColor * aScatteringAbsorption.x, aScatteringAbsorption.x + aScatteringAbsorption.y /* extinction = scattering + absorption */ );
}

float4 OW_FogIntegration_Accumulate( in float4 aFogFront, in float4 aFogBack )
{
	float3 fogColor = aFogBack.rgb * exp2(-aFogFront.a) + aFogFront.rgb;
	return float4(fogColor, aFogFront.a + aFogBack.a);
}

float4 OW_FogIntegration_GetOutput(in float4 aFog)
{
	return float4(aFog.rgb, exp2(-aFog.a));
}
