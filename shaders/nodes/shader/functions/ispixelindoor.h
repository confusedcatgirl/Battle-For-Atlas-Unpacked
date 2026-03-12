import "shaders/MRender/MR_StencilTexture.h"
import "shaders/modules/graphics/stencil.h"
import "shaders/modules/graphics/environment.h"

float IsPixelIndoor(float4 fragCoord : MR_FragCoord)
{
@ifdef GFX_NO_INDOOR
	return 0;	
@else
	bool indoor = (MR_SampleStencilTexelLod0(StencilTexture, fragCoord.xy) & Gfx_Stencil_RoomMask) != 0;
	return indoor ? 1 : 0;
@endif
}