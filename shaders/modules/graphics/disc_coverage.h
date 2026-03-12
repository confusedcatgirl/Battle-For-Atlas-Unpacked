MR_Buffer<float2> 			UVs : MR_Buffer0;
MR_RWBuffer<float> 			Output : MR_RW_Buffer0;
		
float disc_coverage(uint aCurrentPoint, float3 aDirection, float aSize)
{
	float4 position = float4(aDirection, 0);
	float3 worldPosition = MR_Transform(WorldMatrix, position).xyz;
	
	float3 right = cross(aDirection, float3(0, 1, 0));
	float3 up = cross(right, aDirection);
	float2 offset = UVs[aCurrentPoint] * aSize;
	worldPosition += right * offset.x + up * offset.y;
	float3 fromCamera = normalize(worldPosition);

	// sample cloud textures
	float2 uvA;
	float2 uvB;
	float blendComp;
	float4 c_neg;

	CalculateCloudUVs(fromCamera, uvA, uvB, blendComp);
	SampleCloudCoverageNEG(uvA, uvB, blendComp, c_neg);

	// cloud density is in c_neg.a
	return c_neg.a;
}