import "shaders/MRender/MR_DepthBuffer.h"

float3 GetNormal(MR_Texture2D aNormalTexture, int2 pixel)
{
	return Gfx_DecodeNormal_UNorm_Normalized(MR_SampleTexelLod0(aNormalTexture, pixel).xyz);
}

float GetRoughnessFromNormal(MR_Texture2D aNormalTexture, float3 refNormal, int2 pixel)
{
	float3 n = GetNormal(aNormalTexture, pixel);
	return sqrt(saturate(1.0 - dot(refNormal, n)));
}

float GetWeight(float slope, float minslope)
{
	float w = saturate(1.0 - abs(slope - minslope) / (abs(minslope) + 1e-8));

	return w / (slope * slope + 1e-8);
}

float GetRoughnessAA(MR_Texture2D aNormalTexture, int2 pixel)
{
	float3 refNormal = GetNormal(aNormalTexture, pixel);
	float refZ = MR_SampleTexelLod0(DepthBufferZ, pixel);

	int2 offsets[4] = {
		int2(0,-1), int2(0,1),
		int2(-1,0), int2(1,0),
	};

	float slopes[4];
	for (uint i=0; i<4; ++i)
	{
		float z = MR_SampleTexelLod0(DepthBufferZ, pixel + offsets[i]);
		float slope = (i&1) ? z - refZ : refZ - z;
		slopes[i] = slope;
	}

	float minslope_x = abs(slopes[0]) < abs(slopes[1]) ? slopes[0] : slopes[1];
	float minslope_y = abs(slopes[2]) < abs(slopes[3]) ? slopes[2] : slopes[3];

	float rsum = 0;
	float wsum = 1e-8;

	for (uint j=0; j<4; ++j)
	{
		int2 offset = offsets[j];

		float minslope = j<2 ? minslope_x : minslope_y;
		float w = GetWeight(slopes[j], minslope);
		float r = GetRoughnessFromNormal(aNormalTexture, refNormal, pixel + offset);

		rsum += r * w;
		wsum += w;
	}

	return rsum / wsum;
}
