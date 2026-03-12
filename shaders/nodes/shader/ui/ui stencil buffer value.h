import "shaders/modules/graphics/getstencil.h"

pixel float GetStencilBufferValue(float4 fc : MR_FragCoord)
{
	uint stencil = GetStencil(fc.xy);
	return stencil;
}
