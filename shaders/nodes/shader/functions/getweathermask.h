import "shaders/MRender/MR_StencilTexture.h"
import "shaders/modules/graphics/stencil.h"

float UnpackWeatherMask(uint stencil)
{
	float mask = 1 - float(stencil & Gfx_Stencil_NoSnowMask) / Gfx_Stencil_NoSnowMask;
	return mask;
}
	
float GetWeatherMask(float4 fragCoord : MR_FragCoord)
{
	uint stencil = MR_SampleStencilTexelLod0(StencilTexture, fragCoord.xy);

	if (stencil & Gfx_Stencil_Indoor)
		return 0;

	return UnpackWeatherMask(stencil);
}
