import "shaders/modules/graphics/particles/constants.h"

void ParticleTrails(
	float3 position, float3 direction, float size, float safeRegion, float4 colorIn, float2 instanceUV, float ageFrac,
	out float4 clipPos, out float2 uv, out float alpha, out float4 color, out float3 camPos)
{
	float side = instanceUV.y;

	uv = float2(ageFrac, side);

	float invSafe = 1 - safeRegion;
	float valid = ageFrac - invSafe;

	float widthScale = saturate(3 - valid / safeRegion);
	alpha = 1 - (max(0, valid) / safeRegion);
	color = colorIn * float4(1,1,1, alpha);

	float3 cdir = MR_TransformNormal(ourWorldToCamera, direction);
	float2 sideways = normalize(cdir.yx * float2(-1, 1));

	camPos = MR_Transform(ourWorldToCamera, position).xyz;
	camPos.xy += (side - 0.5) * size * sideways;

	clipPos = MR_Transform(ourProjection, camPos);
}
