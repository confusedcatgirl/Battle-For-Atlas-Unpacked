import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/drawer/drawer.h"
import "shaders/modules/graphics/getstencil.h"

pixel float GetStencilBuffer(float2 offset, float4 fc : MR_FragCoord)
{
	uint stencil = GetStencil(fc.xy + offset);
	return stencil;
}
