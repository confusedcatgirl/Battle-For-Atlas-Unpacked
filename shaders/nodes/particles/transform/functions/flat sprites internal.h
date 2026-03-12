import "shaders/modules/graphics/particles/constants.h"
import "shaders/nodes/Shader/Functions/Apply Planet Transforms To Vertex.h"

void ParticleFlatSprites(
	float3 position, float size, float aspectRatio, float2 instanceUV,
	out float4 clipPos, out float3 camPos)
{
	float3 wp = MR_Transform(ourWorldTransform, position).xyz;
	wp = ApplyAllPlanetTransformsToVertex(wp);

	camPos = MR_Transform(MR_ViewConstants.worldToCamera, wp).xyz;
	camPos.xy += (instanceUV - 0.5) * size  * float2(aspectRatio, -1);

	clipPos = MR_Transform(ourProjection, camPos);
}
