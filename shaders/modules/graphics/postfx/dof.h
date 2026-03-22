import "shaders/MRender/MR_DepthBuffer.h"
import "shaders/MRender/MR_ViewConstants.h"

MR_Sampler2D Color : MR_Texture0 { filter = point, wrap = clamp };
MR_Sampler2D ColorLinear : MR_Texture0, 1 { filter = linear, wrap = clamp };

MR_Sampler2D<float> DepthbufferZ : MR_DepthBufferZ
{
	filter = point,
	wrap = clamp,
};

constants DoF : MR_ConstantBuffer1
{
	float Scale1;
	float Bias1;
	float Scale2;
	float Bias2;
	float MinCoC;
	float MaxCoC;
	int MaxCoCSamples;
	int MaxSamples;
}

float GetCoC(float depth)
{
	float coc = (depth * DoF.Scale1 + DoF.Bias1) / (depth * DoF.Scale2 + DoF.Bias2);
	coc = clamp(coc, DoF.MinCoC, DoF.MaxCoC);
	return coc;
}

float GetAbsCoC(float2 aUV, float2 aUVMin, float2 aUVMax, float2 aSampleDirection0, float2 aSampleDirection1)
{
	float depth_center = MR_ZToLinearDepth(MR_SampleLod0(DepthbufferZ, aUV).x);
	float coc_center = GetCoC(depth_center);

	// Box filter seems to produce a better looking CoC
	float coc = coc_center;
	for (int i = 0; i < DoF.MaxCoCSamples; ++i)
	{
		float depth_0 = MR_ZToLinearDepth(MR_SampleLod0(DepthbufferZ, clamp(aUV + aSampleDirection0 * (i + 1), aUVMin, aUVMax)).x);
		float depth_1 = MR_ZToLinearDepth(MR_SampleLod0(DepthbufferZ, clamp(aUV + aSampleDirection1 * (i + 1), aUVMin, aUVMax)).x);
		float coc_0 = GetCoC(depth_0);
		float coc_1 = GetCoC(depth_1);

		coc += abs(coc_0);
		coc += abs(coc_1);
	}
	
	return coc / (1.0 + 2.0 * DoF.MaxCoCSamples);
}

float4 Sample(float2 uv0, float weight, inout float totalWeight)
{
	totalWeight += weight;
	return float4(MR_SampleLod0(Color, uv0).xyz, 1) * weight;
}

float GaussianDistribution(float x, float y, float rho)
{
	float pi = 3.1415926;
	float g = 1.0 / sqrt(2.0 * pi * rho * rho);
	g *= exp(-(x*x + y*y) / (2 * rho * rho));
	return g;
}
