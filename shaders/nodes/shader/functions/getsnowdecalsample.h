import "shaders/modules/graphics/normal_encode.h"

MR_Sampler2D Texture : MR_SnowDecal;

void GetSnowDecalSample(in float4 fc : MR_FragCoord, out float3 normal, out float snowNormalBlend, out float snowMeltingFactor)
{
	float4 snowSample = MR_SampleTexelLod0(Texture, fc.xy);
	float yValue = 1 - saturate(snowSample.x*snowSample.x - snowSample.y*snowSample.y);
	
	yValue = sqrt(max(0.00001, yValue));

	normal = normalize(float3(snowSample.x, yValue, snowSample.y));
	snowNormalBlend =  snowSample.z;
	snowMeltingFactor = snowSample.w;
}