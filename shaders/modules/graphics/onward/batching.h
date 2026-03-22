import "shaders/MRender/MR_ViewConstants.h"

constants : MR_ConstantBuffer0
{
	float myBatchRadius;
	int myBatchStep;

	uint myCustomDataStart;
	uint myCustomDataEnd;
}

uint OW_Batching_GetCustomDataStart( )
{
	return myCustomDataStart;
}

uint OW_Batching_GetCustomDataEnd( )
{
	return myCustomDataEnd;
}

const uint OGFX_BATCHING_SHIFT = 11;
const uint OGFX_BATCHING_MASK = 2047;

MR_Buffer< float4 > PositionBuffer : MR_Buffer0;

uint2 OW_Batching_GetPreCullingIndex( MR_Buffer< uint > aIndexBuffer, in uint anInstanceId )
{
	const uint bufferData = aIndexBuffer[anInstanceId];
	return uint2( (bufferData >> OGFX_BATCHING_SHIFT), anInstanceId - (bufferData & OGFX_BATCHING_MASK) );
}

uint2 OW_Batching_GetPostCullingIndex( MR_Buffer< uint > aIndexBuffer, in uint anInstanceId )
{
	const uint bufferData = aIndexBuffer[anInstanceId];
	return uint2( (bufferData >> OGFX_BATCHING_SHIFT),                (bufferData & OGFX_BATCHING_MASK) );
}

uint OW_Batching_PackIndexForCulling( in uint2 anIndex )
{
	return (anIndex.x << OGFX_BATCHING_SHIFT) | (anIndex.y & OGFX_BATCHING_MASK);
}

float3x4 OW_Batching_GetMatrix( in uint2 anIndex, in int4 aBasisSegment )
{
	float3 instanceSegment = PositionBuffer[ anIndex.x + 0 ].xyz;
	float3 instanceOffset  = PositionBuffer[ anIndex.x + 1 ].xyz;

	float3 batchOffset = ( instanceSegment - aBasisSegment.xyz );
	batchOffset = batchOffset * aBasisSegment.w + instanceOffset;

	const uint firstIndex = ( anIndex.x + 3 ) + anIndex.y * myBatchStep;

	float3x4 objectToWorld = MR_RowMatrix( PositionBuffer[ firstIndex + 0 ] + float4( 0.0f, 0.0f, 0.0f, batchOffset.x ),
	                                       PositionBuffer[ firstIndex + 1 ] + float4( 0.0f, 0.0f, 0.0f, batchOffset.y ),
	                                       PositionBuffer[ firstIndex + 2 ] + float4( 0.0f, 0.0f, 0.0f, batchOffset.z ) );
	return objectToWorld;
}

float3x4 OW_Batching_GetMatrix( MR_Buffer< uint > aIndexBuffer, in uint anInstanceId : MR_InstanceId )
{
	return OW_Batching_GetMatrix( OW_Batching_GetPostCullingIndex( aIndexBuffer, anInstanceId ), MR_ViewConstants.basisSegment );
}
