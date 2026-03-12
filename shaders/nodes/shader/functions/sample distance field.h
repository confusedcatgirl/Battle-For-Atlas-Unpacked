import "shaders/modules/graphics/drawer/drawer.h"

pixel void SampleDistanceField(out float aMaskOut, out float aOutlineOut, MR_Sampler2D aSampler, float2 tc)
{
	float2 t = sampleDistanceField(aSampler, tc);
	aMaskOut = t.x;
	aOutlineOut = t.y;
}
