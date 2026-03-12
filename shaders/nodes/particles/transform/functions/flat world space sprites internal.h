import "shaders/modules/graphics/particles/constants.h"
import "shaders/nodes/Shader/Functions/Apply Planet Transforms To Vertex.h"

void ParticleFlatWorldSpaceSprites(
	float3 position, float3 x, float3 y, float2 instanceUV, bool centerX, bool centerY,
	out float4 clipPos)
{
	float2 offset = float2(0,1) + instanceUV * float2(1,-1);
	if (centerX)
		offset.x -= 0.5;
	if (centerY)
		offset.y -= 0.5;

	position += x * offset.x + y * offset.y;

	float3 wp = MR_Transform(ourWorldTransform, position).xyz;
	wp = ApplyAllPlanetTransformsToVertex(wp);

	clipPos = MR_Transform(MR_ViewConstants.worldToClip, wp);
}
