import "shaders/modules/graphics/particles/constants.h"
import "shaders/nodes/Shader/Functions/Apply Planet Transforms To Vertex.h"

void ParticleFlatTrails(
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

	float3 wp = MR_Transform(ourWorldTransform, position).xyz;
	wp = ApplyAllPlanetTransformsToVertex(wp);

	camPos = MR_Transform(MR_ViewConstants.worldToCamera, wp).xyz;
	camPos.xy += (side - 0.5) * size * sideways;

	clipPos = MR_Transform(ourProjection, camPos);
}
