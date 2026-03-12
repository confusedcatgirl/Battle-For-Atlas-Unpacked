import "shaders/modules/graphics/onward/gi/cosmicdust_struct.h"

MR_RWTexture3D<float4> VolumeTextureRW : MR_RW_Texture0;

constants : MR_ConstantBuffer1
{
	float4 constModifier;
}

void UpdateVolume( in uint3 aVoxelIdx, MR_Buffer< uint > aBuffer, MR_StructuredBuffer< OGI_CosmicDustProbeData > aProbeDataIn )
{
	const uint bufferIndex = OW_GetVoxelBufferIndex( aVoxelIdx );

	const uint probeCount = aBuffer[ bufferIndex ];

	float totalWeight = 0.0;
	float4 total;

	if (probeCount == 0)
	{
		totalWeight = 1.0;
		total = float4(0, 0, 0, 0);
	}
	else
	{
		total = float4(0, 0, 0, 0);
	}

	bool hasError = probeCount > OGI_MAX_PROBES_PER_VOXEL;

	const float3 voxelPos = OW_GetVoxelCenter( aVoxelIdx );
	for (uint it=1;it<=probeCount;++it)
	{
		const uint raw = aBuffer[ bufferIndex + it ];
		const uint idx = ( raw >> 16 ) * constMaxProbesPerSector + ( raw & 0xffff );

		OGI_CosmicDustProbeData probe = aProbeDataIn[ idx ];

		hasError = ( hasError || (probe.myCenterRadius.w == 0.0f) );

		const float3 voxelToProbe = ( probe.myCenterRadius.xyz - voxelPos );
		float weight = rsqrt( max( dot( voxelToProbe, voxelToProbe ), 0.01f ) );
		totalWeight += weight;

		total += (probe.myColorDensity1 + probe.myColorDensity2) * constModifier * weight;
	}

	if (hasError)
	{
		VolumeTextureRW[ aVoxelIdx ] = float4(0, 0, 0, 0);
	}
	else
	{
		VolumeTextureRW[ aVoxelIdx ] = total / max(totalWeight, 1e-6);
	}
}