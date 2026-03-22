struct Gfx_Light
{
	uint roomBits;
	float direction2RcpLengthSquared;
	float direction3RcpLengthSquared;
	float cutoffRange2;

	float3 position;
	float3 direction2;
	float3 direction3;
	float lightSourceRadius;
	float attenuationBias;
	float minNearAttenuationFadeDist;
	float maxNearAttenuationFadeDist;

	float minAttenuationFadeDist;
	float maxAttenuationFadeDist;
	float3 emission;
	uint projectedTextureDataIndex;
	uint angularTextureIndex;
	int shadowMapIndex;
	float influence; // this could probably move to shadow constants if space is needed
	float specScale;

	float3 direction;
	float angleFadeAdd;
	float angleFadeMul;
	float scattering;
};

MR_ByteBuffer Gfx_TileLightList : MR_TileLightList;

Gfx_Light Gfx_UnpackLight(uint4 data[5])
{
	Gfx_Light light;

	uint4 v0 = data[0];
	light.roomBits = v0.x;
	light.direction2RcpLengthSquared = asfloat(v0.y);
	light.direction3RcpLengthSquared = asfloat(v0.z);
	light.cutoffRange2 = asfloat(v0.w);

	uint4 v1 = data[1];
	uint4 v2 = data[2];
	light.position = asfloat(v1.xyz);
	light.direction2.x = f16tof32(v1.w);
	light.direction2.y = f16tof32(v1.w >> 16);
	light.direction2.z = f16tof32(v2.x);
	light.direction3.x = f16tof32(v2.x >> 16);
	light.direction3.y = f16tof32(v2.y);
	light.direction3.z = f16tof32(v2.y >> 16);
	light.lightSourceRadius = f16tof32(v2.z);
	light.attenuationBias = f16tof32(v2.z >> 16);
	light.minNearAttenuationFadeDist = f16tof32(v2.w);
	light.maxNearAttenuationFadeDist = f16tof32(v2.w >> 16);

	uint4 v3 = data[3];
	light.minAttenuationFadeDist = f16tof32(v3.x);
	light.maxAttenuationFadeDist = f16tof32(v3.x >> 16);
	light.emission.x = f16tof32(v3.y);
	light.emission.y = f16tof32(v3.y >> 16);
	light.emission.z = f16tof32(v3.z);
	light.emission *= 1000.0;
	light.projectedTextureDataIndex = (v3.z >> 16) & uint(0xff);
	light.angularTextureIndex = (v3.z >> 24);
	light.influence = (v3.w & uint(0xff)) * (1.0 / 255.0);
	light.specScale = ((v3.w >> 8) & uint(0xff)) * (1.0 / 255.0);

	uint4 v4 = data[4];
	light.direction.x = f16tof32(v4.x);
	light.direction.y = f16tof32(v4.x >> 16);
	light.direction.z = f16tof32(v4.y);
	light.angleFadeAdd = f16tof32(v4.y >> 16);
	light.angleFadeMul = f16tof32(v4.z);
	light.scattering = f16tof32(v4.z >> 16);
	light.shadowMapIndex = v4.w;

	return light;
}

Gfx_Light Gfx_GetLight(MR_ByteBuffer aBuffer, uint aLightIndex)
{
	uint dataIndex = aLightIndex * (5 * 16);

	uint4 data[5];
	data[0] = aBuffer.Load4(dataIndex + (0 * 16));
	data[1] = aBuffer.Load4(dataIndex + (1 * 16));
	data[2] = aBuffer.Load4(dataIndex + (2 * 16));
	data[3] = aBuffer.Load4(dataIndex + (3 * 16));
	data[4] = aBuffer.Load4(dataIndex + (4 * 16));

	return Gfx_UnpackLight(data);
}

Gfx_Light Gfx_GetLightTile(uint aLightIndex)
{
	return Gfx_GetLight(Gfx_TileLightList, aLightIndex);
}

@ifdef GFX_LIGHT_FROM_CONSTANT_BUFFER
	constants Gfx_LightList : MR_LightsConstantBuffer
	{
		uint4 lightData[4096];
	}

	MR_ByteBuffer Gfx_IndirectLightIndices : MR_IndirectLightIndices;

	Gfx_Light Gfx_GetLightFromConstantBuffer(uint aLightIndex)
	{
		uint dataIndex = aLightIndex * 5;

		uint4 data[5];
		data[0] = Gfx_LightList.lightData[dataIndex + 0];
		data[1] = Gfx_LightList.lightData[dataIndex + 1];
		data[2] = Gfx_LightList.lightData[dataIndex + 2];
		data[3] = Gfx_LightList.lightData[dataIndex + 3];
		data[4] = Gfx_LightList.lightData[dataIndex + 4];

		return Gfx_UnpackLight(data);	
	}
@endif
