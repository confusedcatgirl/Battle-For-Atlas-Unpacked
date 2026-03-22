MR_Buffer< float4 > TilesBuffer : MR_Buffer4;
MR_Sampler2D SmallDepth : MR_Texture7 { filter = point, wrap = clamp };

float4 OW_Fog_Tiles_GetData(in uint3 aThreadId, in uint aTileToThread, in uint3 aVolumeSize)
{
	return TilesBuffer[ ((aThreadId.y / aTileToThread) * aVolumeSize.x + aThreadId.x) / aTileToThread ];
}

bool OW_Fog_Tiles_HasCreep(in float4 aData)
{
	return (asuint(aData.x) & 1) == 0;
}

uint OW_FogVolume_GetEarlyZOut(in OW_FogEnvConstants someConstants, in float3 aDir, in float2 aUV, in uint3 aSize, in float4 aTileData)
{
	float4 z = MR_SampleGather(SmallDepth, aUV);
	z.x = min(min(min(z.x, z.y), z.z), z.w);
	if (z.x <= 0.0001)
	{
		return aSize.z;
	}
	else
	{
		float depth = MR_ZToLinearDepth(z.x);
		depth = depth / dot(aDir, MR_ViewConstants.nearPlane.xyz);
		depth = pow(saturate((depth - someConstants.myNear) * someConstants.myInvDistributionRange), someConstants.myDistributionPower);
		return uint(depth * aSize.z + 1.5);
	}
}
