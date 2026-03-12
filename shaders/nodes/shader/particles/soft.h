<shaderType = "Particle">

import "shaders/MRender/MR_ViewConstants.h"
import "shaders/MRender/MR_DepthBuffer.h"
import "shaders/modules/graphics/particles/constants.h"

void SoftParticle(float4 color, float4 shaderParams : Particle_ShaderParams, float4 fc : MR_FragCoord,
	out float4 colorOut, out float alphaOut)
{
	alphaOut = saturate(shaderParams.w);

@ifndef GFX_IS_PARTICLE_SHADOW
	float2 uv = MR_PixelToFramebufferUV(fc.xy);
	float sampledDepth = MR_SampleTexelLod0( Depthbuffer, OriginalToParticleFragCoord(fc.xy) ).x;

	float depth = shaderParams.x;
	float size = shaderParams.y;
	float nearFadeOffset = shaderParams.z;
	if (size > 0.0)
	{
// 		float nearAlpha = saturate((depth + nearFadeOffset) / size);
		float nearAlpha = saturate(depth * 2 / size);

		float2 softness = 1;
		float2 softnessDistance = float2(sampledDepth - depth, 1);
		softness = saturate(softnessDistance * 2 / size);
		alphaOut *= nearAlpha * softness.x /* * softness.y */;
	}
@endif // GFX_IS_PARTICLE_SHADOW

	colorOut = color * float4(1,1,1, alphaOut);

@ifdef OGFX_IS_BACKDROP // ONWARD: Don't use this for backdrop
	colorOut = float4(1,0,1,1);
@endif // OGFX_IS_BACKDROP
}
