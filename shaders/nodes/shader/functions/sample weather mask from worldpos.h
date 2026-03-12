import "shaders/nodes/Shader/Functions/Sample Weather Mask.h"

float3 SampleWeatherMaskFromWorldPos(float3 aWorldPos)
{
	float2 uv = saturate(float2((aWorldPos.x * Gfx_Environment.weatherMaskWorldSize.x) + Gfx_Environment.weatherMaskWorldSize.y, (aWorldPos.z * Gfx_Environment.weatherMaskWorldSize.z) + Gfx_Environment.weatherMaskWorldSize.w));
	return SampleWeatherMask(uv);
}