import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/modules/graphics/onworld/planet_utils.h"

float Planet_GetCurstMinStaticRadius() { return OW_PlanetConstants.const_PlanetRadii.x; }
float Planet_GetCurstMaxStaticRadius() { return OW_PlanetConstants.const_PlanetRadii.y; }
float Planet_GetOceanStaticRadius() { return OW_PlanetConstants.const_PlanetRadii.z; }

float EvalSmoothedRadius(in float radius, in float smoothedRadius)
{
    return lerp(radius, smoothedRadius, Planet_GetSmoothingFactor());
}

float4 Planet_CameraToClipSpace(in float3 pos)
{
    float4 clipPos = MR_Transform(MR_ViewConstants.cameraToClip, pos);
    OW_Transform_ClipZ(clipPos);
    clipPos.z = lerp(clipPos.z, 0.0f, OW_Planet_GetIsBackdrop());
    return clipPos;
}

float4 Planet_WorldToClipSpace(in float3 pos)
{
    float4 clipPos = MR_Transform(MR_ViewConstants.worldToClip, pos);
    OW_Transform_ClipZ(clipPos);
    clipPos.z = lerp(clipPos.z, 0.0f, OW_Planet_GetIsBackdrop());
    return clipPos;
}