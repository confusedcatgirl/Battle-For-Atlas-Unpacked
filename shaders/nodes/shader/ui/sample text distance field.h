import "shaders/modules/graphics/drawer/drawer.h"

pixel void SampleDistanceField(out float aAlpha, MR_Sampler2D aSampler, float2 tc)
{
	aAlpha = sampleMask(aSampler, tc).r;
}
