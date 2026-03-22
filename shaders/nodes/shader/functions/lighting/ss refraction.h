import "shaders/modules/graphics/ssr_common.h"
import "shaders/nodes/Shader/Functions/Lighting/Environment Map.h"

float4 SSR_GetUVRefraction(
	MR_Sampler2D aFramebufferSampler,
	float2 aUV0,
	float2 aUV,
	float4 MR_FragCoord : MR_FragCoord
)
{
	float2 uvdiff = aUV - aUV0;
	if (dot(uvdiff, uvdiff) < 0.5 / (MR_ViewConstants.framebufferSize.x * MR_ViewConstants.framebufferSize.x))
		return vec4(0);
		
	if((aUV.x < MR_ViewConstants.framebufferUVMin.x) || (aUV.y < MR_ViewConstants.framebufferUVMin.y) ||
       (aUV.x > MR_ViewConstants.framebufferUVMax.x) || (aUV.y > MR_ViewConstants.framebufferUVMax.y))
		return vec4(0);
	   
	float depth0 = MR_FragCoord.w;
	float depth = MR_SampleLod0(Depthbuffer, aUV).x;
	if (depth < depth0)
		return vec4(0);

	return float4(MR_SampleLod0(aFramebufferSampler, aUV).xyz, 1);
}

float4 SSR_GetRefraction(
	MR_Sampler2D aFramebufferSampler,
	float3 aNormal,
	float aAlpha,
	float aIndexFactor,
	float aMaxDistance,
	float3 Gfx_WorldSpacePos : Gfx_WorldSpacePos,
	float4 MR_FragCoord : MR_FragCoord
)
{
	float3 vertexToCamera = normalize(MR_GetPos(MR_ViewConstants.cameraToWorld).xyz - Gfx_WorldSpacePos);

	float3 neutralDirection = -vertexToCamera;
	float3 normalDirection = -aNormal;

	float3 direction = normalize(lerp(normalDirection, neutralDirection, aIndexFactor));

	float3 position = Gfx_WorldSpacePos + aAlpha * aMaxDistance * direction;
	float4 clipPos = MR_Transform(MR_ViewConstants.worldToClip, position);

	float2 uv0 = MR_PixelToFramebufferUV(MR_FragCoord.xy);
	float2 uv = MR_ClipToFramebufferUV(clipPos.xy / clipPos.w);

	return SSR_GetUVRefraction(aFramebufferSampler, uv0, uv, MR_FragCoord);
}

void SSR_FriendlyRefraction(
	out float4 aLightOut,
	float3 aNormal,
	float aAlpha,
	float aIndexFactor,
	float aMaxDistance,
	float3 Gfx_WorldSpacePos : Gfx_WorldSpacePos,
	float4 MR_FragCoord : MR_FragCoord
)
{
	aLightOut = SSR_GetRefraction(SSR_FramebufferLate, aNormal, aAlpha, aIndexFactor, aMaxDistance, Gfx_WorldSpacePos, MR_FragCoord);
}
