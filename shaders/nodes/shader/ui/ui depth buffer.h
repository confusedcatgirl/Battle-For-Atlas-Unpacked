import "shaders/modules/graphics/getdepth.h"

pixel float UIGetDepth(float4 fc : MR_FragCoord)
{
	return GetDepth(fc.xy);
}
