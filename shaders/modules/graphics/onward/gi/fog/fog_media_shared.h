import "shaders/modules/graphics/onward/gi/fog/fog_shared.h"
import "shaders/modules/graphics/onward/gi/fog/fog_tiles.h"

MR_RWTexture3D<float4> FogMediaVolumeRW : MR_RW_Texture0;
MR_RWTexture3D<float2> PropertyVolumeRW : MR_RW_Texture1;

@ifdef OGI_FOG_REPROJECT
	MR_Texture3D<float4> LastFrameFogMedia : MR_Texture0;
	MR_Sampler3D<float4> LastFrameFogMediaSampler : MR_Texture1 { filter = linear point, wrap = clamp, anisotropy = 0 };
	MR_Texture3D<float2> LastFrameProperty : MR_Texture2;
	MR_Sampler3D<float2> LastFramePropertySampler : MR_Texture3 { filter = linear point, wrap = clamp, anisotropy = 0 };
@endif // OGI_FOG_REPROJECT

constants : MR_ConstantBuffer6
{
	float3 constJitter;
	uint constTileToThread;

	uint3  constColorVolumeSize;
	uint   constFirstVolume1;
	float3 constColorVolumeInvSize;
	uint   constFirstVolume2;
	float4 constParams;
	float4 constHarvesterFog1;
	float4 constHarvesterFog2;
	float4 constHeightParams;
	float4 constCloudsParams;
}

struct OGI_Volume
{
	float3x4 transform;

	float4 colorDensity;
	int nextIndex;
};

OGI_Volume OGI_UnpackVolume(uint4 data[4])
{
	OGI_Volume volume;

	volume.transform[0] = asfloat(data[0].xyzw);
	volume.transform[1] = asfloat(data[1].xyzw);
	volume.transform[2] = asfloat(data[2].xyzw);

	volume.colorDensity.xyz = asfloat(data[3]).xyz;

	volume.colorDensity.w = f16tof32(data[3].w);
	volume.nextIndex = data[3].w >> 16;

	return volume;
}

@ifdef OGI_USE_VOLUMES_FROM_CONSTANTBUFFER

	constants OGI_VolumeBuffer : MR_ConstantBuffer5
	{
		uint4 volumeData[4096];
	}

	OGI_Volume OGI_GetVolume(uint aVolumeIndex)
	{
		uint dataIndex = aVolumeIndex * 4;

		uint4 data[4];
		data[0] = OGI_VolumeBuffer.volumeData[dataIndex + 0];
		data[1] = OGI_VolumeBuffer.volumeData[dataIndex + 1];
		data[2] = OGI_VolumeBuffer.volumeData[dataIndex + 2];
		data[3] = OGI_VolumeBuffer.volumeData[dataIndex + 3];
		return OGI_UnpackVolume(data);
	}

@else

	MR_ByteBuffer OGI_VolumeBuffer : MR_Buffer0;

	OGI_Volume OGI_GetVolume(uint aVolumeIndex)
	{
		uint dataIndex = aVolumeIndex * (4 * 16);

		uint4 data[4];
		data[0] = OGI_VolumeBuffer.Load4(dataIndex + (0*16));
		data[1] = OGI_VolumeBuffer.Load4(dataIndex + (1*16));
		data[2] = OGI_VolumeBuffer.Load4(dataIndex + (2*16));
		data[3] = OGI_VolumeBuffer.Load4(dataIndex + (3*16));
		return OGI_UnpackVolume(data);
	}

@endif // OGI_USE_VOLUMES_FROM_CONSTANTBUFFER

struct OW_FogColor_Media
{
	float4 myColorXYZAnisotropyW;
	float2 myScatteringXAbsorptionY;
};

void OW_FogColor_SetMedia( inout OW_FogColor_Media aMediaOut, in OW_FogColor_Media aMediaIn )
{
	aMediaOut.myColorXYZAnisotropyW = aMediaIn.myColorXYZAnisotropyW * aMediaIn.myScatteringXAbsorptionY.x;
	aMediaOut.myScatteringXAbsorptionY = aMediaIn.myScatteringXAbsorptionY;
}

void OW_FogColor_AddMedia( inout OW_FogColor_Media aMediaOut, in OW_FogColor_Media aMediaIn )
{
	aMediaOut.myColorXYZAnisotropyW += aMediaIn.myColorXYZAnisotropyW * aMediaIn.myScatteringXAbsorptionY.x;
	aMediaOut.myScatteringXAbsorptionY += aMediaIn.myScatteringXAbsorptionY;
}

void OW_FogColor_AddMedia( inout OW_FogColor_Media aMediaOut, in float4 aColorXYZScatteringW, in float anAnisotropy )
{
	aMediaOut.myColorXYZAnisotropyW += float4(aColorXYZScatteringW.xyz, anAnisotropy) * aColorXYZScatteringW.w;
	aMediaOut.myScatteringXAbsorptionY.x += aColorXYZScatteringW.w;
}

void OW_FogColor_Normalize( inout OW_FogColor_Media aMediaOut )
{
	aMediaOut.myColorXYZAnisotropyW /= aMediaOut.myScatteringXAbsorptionY.x;
	aMediaOut.myColorXYZAnisotropyW = max(aMediaOut.myColorXYZAnisotropyW, vec4(0.0));
}

void OW_FogColor_AddVolumes( in float3 aPosition, inout OW_FogColor_Media aMediaOut, in float anAnisotropy )
{
@ifndef OGI_FOG_NO_VOLUMES_1
	uint boxIndex = constFirstVolume1;
	while( boxIndex != 65535 )
	{
		OGI_Volume volume = OGI_GetVolume( boxIndex );
		const float3 p = MR_Transform( volume.transform, aPosition ); // transform the position into local space
		float3 t = abs( p );
		float d = length( max( t.x, max(t.y, t.z) ) );
		OW_FogColor_AddMedia( aMediaOut, volume.colorDensity * saturate(1.0f - d), anAnisotropy );
		boxIndex = volume.nextIndex; // next
	}
@endif // !OGI_FOG_NO_VOLUMES_1

@ifndef OGI_FOG_NO_VOLUMES_2
	uint ellipsoidIndex = constFirstVolume2;
	while( ellipsoidIndex != 65535 )
	{
		OGI_Volume volume = OGI_GetVolume( ellipsoidIndex );
		const float3 p = MR_Transform( volume.transform, aPosition ); // transform the position into local space
		float d = length( p );
		OW_FogColor_AddMedia( aMediaOut, volume.colorDensity * saturate(1.0f - d), anAnisotropy );
		ellipsoidIndex = volume.nextIndex; // next
	}
@endif // !OGI_FOG_NO_VOLUMES_1
}
