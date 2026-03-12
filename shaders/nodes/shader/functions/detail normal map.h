import "shaders/nodes/Shader/Functions/Unpack Normal.h"

@ifdef MATERIAL_OVERRIDE
import "shaders/nodes/Shader/Functions/Lighting/Material Override.h"
@endif

void ApplyDetailNormal(out float3 aNormalOut, out float aAOOut,
		       float3 aNormal, float aAO,
		       MR_Sampler2D aDetailSampler, float2 aTiling, float aScale <default = 1>, float2 aUV : Gfx_UV0)
{
@ifdef GFX_DEBUG_NO_DETAIL_NORMALMAP
	aNormalOut = aNormal;
	aAOOut = aAO;
@else
	@ifdef MATERIAL_OVERRIDE
		if (MR_MaterialOverride.isEnabled && MR_MaterialOverride.overrideDetailNormalMap)
			aScale = MR_MaterialOverride.replacementDetailNormalMapScale;
	@endif

	float4 packedNormal = MR_Sample(aDetailSampler, aUV * aTiling);
	float3 n;
	float ao;
	UnpackNormalAndAO(packedNormal, n, ao);
	aNormalOut = lerp(aNormal, aNormal * float3(1,1,0.9) + n * float3(1.2,1.2,0), aScale);
	aAOOut = lerp(aAO, min(aAO, ao), aScale);
@endif
}
