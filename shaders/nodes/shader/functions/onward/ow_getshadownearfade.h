import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/shadows_common.h"

float OW_GetShadowNearFade(in float3 aWorldPosition)
{
	if (MR_ViewConstants.isShadowView == 0)
	{
		return 1.0;
	}

	const float dist = dot(ShadowConstants.fadingPlane.xyz, aWorldPosition) + ShadowConstants.fadingPlane.w;
	return saturate(ShadowConstants.fadingRange * dist) * OW_ViewConstants.myShadowNearFadeFactor;

}
