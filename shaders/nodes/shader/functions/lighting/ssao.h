import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/global_samplers.h"

float SSAO_GetVisibility(float4 fc : MR_FragCoord)
{
    @ifdef SSAO
        return MR_SampleTexelLod0(SSAO, int2(fc.xy)).x;
    @else
        return 1.0;
    @endif
}

void SSAO_CombineVisibility(inout float visibility, float ssao)
{
@ifdef SSAO
    visibility = min(visibility, ssao);
@endif
}

void SSAO_CombineVisibility(inout float visibility <default = 1>, float4 fc : MR_FragCoord)
{
@ifdef SSAO
    visibility = min(visibility, SSAO_GetVisibility(fc));
@endif
}
