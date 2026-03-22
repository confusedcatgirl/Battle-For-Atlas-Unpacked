import "shaders/nodes/Shader/Functions/Lighting/SS Refraction.h"

void SSR_FriendlyRefractionEarly(
	out float4 aLightOut,
	float3 aNormal,
	float aAlpha,
	float aIndexFactor,
	float aMaxDistance,
	float3 Gfx_WorldSpacePos : Gfx_WorldSpacePos,
	float4 MR_FragCoord : MR_FragCoord
)
{
	aLightOut = SSR_GetRefraction(SSR_FramebufferEarly, aNormal, aAlpha, aIndexFactor, aMaxDistance, Gfx_WorldSpacePos, MR_FragCoord);
}
