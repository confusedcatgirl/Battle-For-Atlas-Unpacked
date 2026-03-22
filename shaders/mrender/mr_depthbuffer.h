const float MAX_LINEAR_DEPTH = 65504.0f;

MR_Sampler2D<float> Depthbuffer : MR_DepthTexture
{
	filter = point,
	wrap = clamp,
};

MR_Sampler2D<float> QuarterDepthbuffer : MR_QuarterDepthTexture
{
	filter = point,
	wrap = clamp,
};

MR_Texture2D<float> DepthBufferZ : MR_DepthBufferZ;

void MR_DepthBuffer_Dummy()
{
// implemented this way so no include dependency is added, needs to be handled if the macro is used
#ifdef IS_DURANGO
#define MR_SampleLinearDepth(pixel) MR_ZToLinearDepth(MR_SampleTexelLod0(DepthBufferZ, pixel))
#else
#define MR_SampleLinearDepth(pixel) MR_SampleTexelLod0(Depthbuffer, pixel);
#endif
}
