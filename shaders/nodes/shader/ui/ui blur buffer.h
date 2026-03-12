import "shaders/modules/graphics/getblurredbackbuffer.h"

pixel float3 UIGetBlur(float4 fragCoord : MR_FragCoord)
{
	return GetBlurBuffer(fragCoord.xy);
}
