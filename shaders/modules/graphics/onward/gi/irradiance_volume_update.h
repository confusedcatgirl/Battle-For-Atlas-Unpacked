import "shaders/modules/graphics/onward/gi/irradiance_struct.h"

MR_RWTexture3D<float3> VolumeTextureRW0 : MR_RW_Texture0;
MR_RWTexture3D<float3> VolumeTextureRW1 : MR_RW_Texture1;
MR_RWTexture3D<float3> VolumeTextureRW2 : MR_RW_Texture2;
@ifndef OGI_MERGE_VOLUME_BASES
	MR_RWTexture3D<float3> VolumeTextureRW3 : MR_RW_Texture3;
	MR_RWTexture3D<float3> VolumeTextureRW4 : MR_RW_Texture4;
	MR_RWTexture3D<float3> VolumeTextureRW5 : MR_RW_Texture5;
@endif // OGI_MERGE_VOLUME_BASES

constants : MR_ConstantBuffer1
{
	float4 constFallback[ 6 ];
}

void UpdateVolume( in uint3 aVoxelIdx, MR_Buffer< uint > aBuffer, MR_StructuredBuffer< OGI_IrradianceProbeData > aProbeDataIn )
{
	const uint bufferIndex = OW_GetVoxelBufferIndex( aVoxelIdx );

	const uint probeCount = aBuffer[ bufferIndex ];

	float totalWeight = 0.0f;
	float3 totalIrradiance[6];

	if (probeCount == 0)
	{
		totalWeight = 1.0f;

		totalIrradiance[ 0 ] = constFallback[ 0 ].xyz;
		totalIrradiance[ 1 ] = constFallback[ 1 ].xyz;
		totalIrradiance[ 2 ] = constFallback[ 2 ].xyz;
		totalIrradiance[ 3 ] = constFallback[ 3 ].xyz;
		totalIrradiance[ 4 ] = constFallback[ 4 ].xyz;
		totalIrradiance[ 5 ] = constFallback[ 5 ].xyz;
	}
	else
	{
		totalIrradiance[ 0 ] = vec3(0);
		totalIrradiance[ 1 ] = vec3(0);
		totalIrradiance[ 2 ] = vec3(0);
		totalIrradiance[ 3 ] = vec3(0);
		totalIrradiance[ 4 ] = vec3(0);
		totalIrradiance[ 5 ] = vec3(0);
	}

	bool hasError = probeCount > OGI_MAX_PROBES_PER_VOXEL;

	const float3 voxelPos = OW_GetVoxelCenter( aVoxelIdx );
	for (uint it=1;it<=probeCount;++it)
	{
		const uint raw = aBuffer[ bufferIndex + it ];
		const uint idx = ( raw >> 16 ) * constMaxProbesPerSector + ( raw & 0xffff );

		OGI_IrradianceProbeData probe = aProbeDataIn[ idx ];

		hasError = ( hasError || (probe.myCenterRadius.w == 0.0f) );

		const float3 voxelToProbe = ( probe.myCenterRadius.xyz - voxelPos );
		float weight = rsqrt( max( dot( voxelToProbe, voxelToProbe ), 0.01f ) );
		totalWeight += weight;

		totalIrradiance[ 0 ] += probe.myIrradiance[ 0 ] * weight;
		totalIrradiance[ 1 ] += probe.myIrradiance[ 1 ] * weight;
		totalIrradiance[ 2 ] += probe.myIrradiance[ 2 ] * weight;
		totalIrradiance[ 3 ] += probe.myIrradiance[ 3 ] * weight;
		totalIrradiance[ 4 ] += probe.myIrradiance[ 4 ] * weight;
		totalIrradiance[ 5 ] += probe.myIrradiance[ 5 ] * weight;
	}

	if (hasError)
	{
		@ifdef OGI_MERGE_VOLUME_BASES
			const uint3 _voxelIdx = uint3(aVoxelIdx.x, aVoxelIdx.y, constVoxelCount.z+aVoxelIdx.z);
			VolumeTextureRW0[ _voxelIdx ] = float3(10, 0, 0);
			VolumeTextureRW0[ aVoxelIdx ] = float3(10, 0, 0);
			VolumeTextureRW1[ _voxelIdx ] = float3(10, 0, 0);
			VolumeTextureRW1[ aVoxelIdx ] = float3(10, 0, 0);
			VolumeTextureRW2[ _voxelIdx ] = float3(10, 0, 0);
			VolumeTextureRW2[ aVoxelIdx ] = float3(10, 0, 0);
		@else
			VolumeTextureRW0[ aVoxelIdx ] = float3(10, 0, 0);
			VolumeTextureRW1[ aVoxelIdx ] = float3(10, 0, 0);
			VolumeTextureRW2[ aVoxelIdx ] = float3(10, 0, 0);
			VolumeTextureRW3[ aVoxelIdx ] = float3(10, 0, 0);
			VolumeTextureRW4[ aVoxelIdx ] = float3(10, 0, 0);
			VolumeTextureRW5[ aVoxelIdx ] = float3(10, 0, 0);
		@endif // OGI_MERGE_VOLUME_BASES
	}
	else
	{
			totalWeight = max(totalWeight, 1e-6);

		@ifdef OGI_MERGE_VOLUME_BASES
			const uint3 _voxelIdx = uint3(aVoxelIdx.x, aVoxelIdx.y, constVoxelCount.z+aVoxelIdx.z);
			VolumeTextureRW0[ _voxelIdx ] = totalIrradiance[ 0 ] / totalWeight;
			VolumeTextureRW0[ aVoxelIdx ] = totalIrradiance[ 1 ] / totalWeight;
			VolumeTextureRW1[ _voxelIdx ] = totalIrradiance[ 2 ] / totalWeight;
			VolumeTextureRW1[ aVoxelIdx ] = totalIrradiance[ 3 ] / totalWeight;
			VolumeTextureRW2[ _voxelIdx ] = totalIrradiance[ 4 ] / totalWeight;
			VolumeTextureRW2[ aVoxelIdx ] = totalIrradiance[ 5 ] / totalWeight;
		@else
			VolumeTextureRW0[ aVoxelIdx ] = totalIrradiance[ 0 ] / totalWeight;
			VolumeTextureRW1[ aVoxelIdx ] = totalIrradiance[ 1 ] / totalWeight;
			VolumeTextureRW2[ aVoxelIdx ] = totalIrradiance[ 2 ] / totalWeight;
			VolumeTextureRW3[ aVoxelIdx ] = totalIrradiance[ 3 ] / totalWeight;
			VolumeTextureRW4[ aVoxelIdx ] = totalIrradiance[ 4 ] / totalWeight;
			VolumeTextureRW5[ aVoxelIdx ] = totalIrradiance[ 5 ] / totalWeight;
		@endif // OGI_MERGE_VOLUME_BASES
	}
}