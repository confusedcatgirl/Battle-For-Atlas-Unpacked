import "shaders/modules/graphics/getstencil.h"

// return alpha 1 where player is
pixel float GetStencilBuffer(float4 fc : MR_FragCoord)
{
	uint stencil = GetStencil(fc.xy);
	if(stencil == 1)
		return 1;
	return 0;
}
