@ifndef OGI_FOG_NO_LIGHTS
import "shaders/modules/graphics/lights_common.h"
import "shaders/modules/graphics/spot_shadows.h"
@endif // !OGI_FOG_NO_LIGHTS
import "shaders/modules/graphics/onward/gi/fog/fog_tiles.h"
import "shaders/modules/graphics/onward/gi/fog/fog_light.h"

MR_RWTexture3D<float3> FogLightVolumeRW : MR_RW_Texture0;
MR_RWTexture3D<float3> LitMediaVolumeRW : MR_RW_Texture1;

MR_Texture3D<float4> FogMediaTexture : MR_Texture0;

constants : MR_ConstantBuffer7
{
	float3 constJitter;
	uint constTileToThread;

	uint3  constLightVolumeSize;
	int    constLocalLightCount;
	float3 constLightVolumeInvSize;
	float4 constLightShaftsParams;
	float3 constCoverageCheck;
	int    constHasCoverageCheck;
}

@ifndef OGI_FOG_NO_LIGHTS

@ifdef OGI_FOG_USE_LIGHTS_FROM_CONSTANTBUFFER

constants : MR_ConstantBuffer5
{
	uint4 constLocalLightData[4096];
}

Gfx_Light OW_FogLight_GetLightFromConstantBuffer( uint aLightIndex )
{
	uint dataIndex = aLightIndex * 5;

	uint4 data[5];

	data[0] = constLocalLightData[dataIndex + 0];
	data[1] = constLocalLightData[dataIndex + 1];
	data[2] = constLocalLightData[dataIndex + 2];
	data[3] = constLocalLightData[dataIndex + 3];
	data[4] = constLocalLightData[dataIndex + 4];

	return Gfx_UnpackLight(data);
}

@else

MR_ByteBuffer LocalLightData : MR_Buffer0;

@endif // OGI_FOG_USE_LIGHTS_FROM_CONSTANTBUFFER

float3 OW_FogLight_GetLocalLights( in float3 aPos, in float3 aViewDir, in float anAnisotropy )
{
	float3 radiance = 0.0f;
	for (uint lightIdx=0;lightIdx<constLocalLightCount;++lightIdx)
	{
		@ifdef OGI_FOG_USE_LIGHTS_FROM_CONSTANTBUFFER
			const Gfx_Light light = OW_FogLight_GetLightFromConstantBuffer( lightIdx );
		@else
			const Gfx_Light light = Gfx_GetLight( LocalLightData, lightIdx );
		@endif // OGI_FOG_USE_LIGHTS_FROM_CONSTANTBUFFER

		Gfx_AttenuatedEmission attenuatedEmission;
		if (Gfx_GetSpotShadowAttenuatedEmission( attenuatedEmission, light, aPos, vec2(0) ))
		{
			float3 toLightDirection = normalize( attenuatedEmission.diffuseToLight );
			float scattering = OW_FogLight_GetPhaseFunction( dot( toLightDirection, aViewDir ), anAnisotropy );
			radiance.rgb += attenuatedEmission.diffuseEmission.rgb * scattering;
		}
	}
	return radiance;
}

@else

float3 OW_FogLight_GetLocalLights( in float3 aPos, in float3 aViewDir, in float anAnisotropy )
{
	return vec3( 0.0f );
}

@endif // !OGI_FOG_NO_LIGHTS

@ifdef OGI_FOG_USE_SUN_SHAFTS

MR_Sampler3D LightShaftsTexture : MR_Texture2 { filter = linear point, wrap = border, border = 0 0 0 0 };

float OW_FogLight_GetLightShafts( in float2 aTexCoord, in float aDepth )
{
	float sliceDistance = constLightShaftsParams.z * constLightShaftsParams.y;
	float sampleDepth = ( aDepth - sliceDistance ) * constLightShaftsParams.w;
	sampleDepth = min( sampleDepth, constLightShaftsParams.x );

	return MR_SampleLod0( LightShaftsTexture, float3( aTexCoord, sampleDepth ) ).x;
}

@endif // OGI_FOG_USE_SUN_SHAFTS

