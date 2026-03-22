import "shaders/modules/graphics/getstencil.h"

// Discard where player is
pixel float GetStencilBuffer(float4 fc : MR_FragCoord)
{
	uint stencil = GetStencil(fc.xy);
	if(stencil == 1)
		discard;
	return 1;
}
