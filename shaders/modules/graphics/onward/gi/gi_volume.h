import "shaders/modules/graphics/onward/gi/gi_constants.h"

constants : MR_ConstantBuffer0
{
	float3 constVolumePos;
	float constFreeFloat0;

	int3 constRecyclingOffset;
	float constFreeFloat1;

	int3 constVoxelCount;
	uint constSectorCount;

	float3 constVoxelSize;
	uint constMaxProbesPerSector;

	uint4 constSectorParams[ 65 ];
}

uint OW_GetVoxelLinearIndex( in uint3 aVoxelIdx )
{
	return ( aVoxelIdx.z * constVoxelCount.y + aVoxelIdx.y ) * constVoxelCount.x + aVoxelIdx.x;
}

uint OW_GetVoxelBufferIndex( in uint3 aVoxelIdx )
{
	return OW_GetVoxelLinearIndex( aVoxelIdx ) * ( OGI_MAX_PROBES_PER_VOXEL + 1 );
}

float3 OW_GetVoxelPosition( in uint3 aVoxelIdx )
{
	return ( aVoxelIdx * constVoxelSize ) + constVolumePos;
}

float3 OW_GetVoxelCenter( in uint3 aVoxelIdx )
{
	return OW_GetVoxelPosition( aVoxelIdx ) + constVoxelSize * 0.5f;
}