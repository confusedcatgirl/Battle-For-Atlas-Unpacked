constants : MR_ConstantBuffer0
{
	float4x4 WorldMatrix;
	float TexelSize;
	float CoordinateScale;
	float HeightScale;
	float LodDistance;
}

constants TileData : MR_ConstantBuffer1
{
	float2 tileScale;
	float2 tileBias;
	float2 maskScale;
	float2 maskBias;
}
