import "shaders/modules/graphics/particles/constants.h"

void ParticleSprites(
	float3 position, float size, float aspectRatio, float2 instanceUV,
	out float4 clipPos, out float3 camPos)
{
	camPos = MR_Transform(ourWorldToCamera, position).xyz;
	camPos.xy += (instanceUV - 0.5) * size * float2(aspectRatio, -1);

	clipPos = MR_Transform(ourProjection, camPos);
}
