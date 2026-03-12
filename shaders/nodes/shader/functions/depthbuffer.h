import "shaders/modules/graphics/getdepth.h"

pixel float GetDepthbuffer(float4 fc : MR_FragCoord)
{
	return GetDepth(fc.xy);
}
