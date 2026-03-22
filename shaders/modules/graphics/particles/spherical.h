import "shaders/modules/graphics/particles/constants.h"

float GetSelfShadowingFromDot(float dot_)
{
	return saturate(dot_ * 0.7 + 0.7);
}

float GetSelfShadowingFactor(float3 pos, float3 toSunDirection, float scale)
{
	if (ourLightSphere.w <= 0)
		return 1.0;

	float3 distanceFromCenter = (pos - ourLightSphere.xyz) * scale / ourLightSphere.w;
	float dot_ = dot(distanceFromCenter, toSunDirection);
	return GetSelfShadowingFromDot(dot_);
}
