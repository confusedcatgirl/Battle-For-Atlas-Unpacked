import "shaders/modules/graphics/environment_constants.h"

float4 OWParticleIntensityMultiplier(in float4 aUnfoggedColor)
{
    aUnfoggedColor.rgb *= Gfx_Environment.particleIntensityMultiplier;
    return aUnfoggedColor;
}