import "shaders/modules/graphics/particles/constants.h"

void ParticleWorldSpaceSprites(
	float3 position, float3 x, float3 y, float2 instanceUV, bool centerX, bool centerY,
	out float4 clipPos)
{
	float2 offset = float2(0,1) + instanceUV * float2(1,-1);
	if (centerX)
		offset.x -= 0.5;
	if (centerY)
		offset.y -= 0.5;

	position += x * offset.x + y * offset.y;

	clipPos = MR_Transform(ourWorldToClip, position);
}
