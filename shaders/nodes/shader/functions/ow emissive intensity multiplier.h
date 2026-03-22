import "shaders/modules/graphics/environment_constants.h"

float4 OWParticleIntensityMultiplier(float4 inColor)
{
    float4 outVal = inColor;
    outVal.rgb *= Gfx_Environment.particleIntensityMultiplier;
    return outVal;
}

float4 OWEmissiveIntensityMultiplier(in float4 aUnfoggedColor)
{
    aUnfoggedColor.rgb *= Gfx_Environment.emissiveIntensityMultiplier;
    return aUnfoggedColor;
}