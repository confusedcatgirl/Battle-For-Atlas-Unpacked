import "shaders/MRender/MR_ViewConstants.h"

MR_Sampler2D<float3> Reflection : MR_PlaneReflection { wrap = clamp, filter = point };

void GetMirrorReflection(out float3 aLightOut, float4 fc : MR_FragCoord)
{
	// TODO: special-case with load instead of sample for consoles?

	float2 uv = MR_PixelToClip(fc.xy) * 0.5 + 0.5;
	aLightOut = MR_SampleLod0(Reflection, uv);
}
