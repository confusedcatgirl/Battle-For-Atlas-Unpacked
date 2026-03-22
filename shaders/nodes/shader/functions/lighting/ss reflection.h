import "shaders/modules/graphics/ssr_common.h"
import "shaders/modules/graphics/environment.h"
import "shaders/nodes/Shader/Functions/Lighting/Environment Map.h"

bool SSR_Sample(float3 aRayPos,
	out float2 aUVOut, out float aDepthDiffOut, out float aCalculatedDepthOut
)
{
	float4 clipPos = MR_Transform(MR_ViewConstants.worldToClip, float4(aRayPos, 1));
	aCalculatedDepthOut = clipPos.w;

	if (abs(clipPos.x / clipPos.w) >= 1 || abs(clipPos.y / clipPos.w) >= 1)
		return false;

	float2 clipPos2 = clipPos.xy / clipPos.w;

	float2 uv = MR_ClipToFramebufferUV(clipPos2);
	float2 quv = MR_ClipToQuarterFramebufferUV(clipPos2);
	float depth = MR_SampleLod0(QuarterDepthbuffer, quv).x;

	aUVOut = uv;
	aDepthDiffOut = depth - clipPos.w;
	return true;
}

bool SSR_CheckSample(float3 aStartPos, float3 aDirection, float3 aNormal, float3 aRayPos, float aDepthDiff)
{
	float4 clipPos = MR_Transform(MR_ViewConstants.worldToClip, float4(aRayPos, 1));
	float sampledDepth = aDepthDiff + clipPos.w;

	float3 worldPos = MR_ClipToWorld(clipPos.xy / clipPos.w, sampledDepth);
	float3 reflectionVector = worldPos - aStartPos;

	if (dot(normalize(reflectionVector), aDirection) < 0.5)
		return false;

	if (dot(reflectionVector, aNormal) < 0.1)
		return false;

	return true;
}

float4 SSR_GetReflectionSingle(
	MR_Sampler2D aFramebufferSampler,
	float3 aStartPos,
	float4 MR_FragCoord : MR_FragCoord,
	float3 aDirection,
	float3 aNormal,
	float aQualityFactor,
	float aRangeFactor,
	float aStepFactor,
	out float3 aReflectionPosOut
)
{
	aReflectionPosOut = aStartPos;
	return float4(10.0, 0.0, 10.0, 0.0);
}

float4 SSR_GetReflection(
	MR_Sampler2D aFramebufferSampler,
	float3 aStartPos,
	float4 MR_FragCoord : MR_FragCoord,
	float3 aDirection,
	float3 aNormal,
	float aQualityFactor,
	float aRangeFactor,
	float aStepFactor
)
{
@ifndef GFX_NO_SS_REFLECTIONS
	float3 posOut;
	return SSR_GetReflectionSingle(aFramebufferSampler, aStartPos, MR_FragCoord, aDirection, aNormal, aQualityFactor, aRangeFactor, aStepFactor, posOut);
@endif
	return vec4(0);
}

void SSR_FriendlyReflection(
	out float4 aLightOut,
	MR_Sampler2D aFramebufferSampler,
	float3 aNormal,
	float aRoughness,
	float aUseReflection,
	float3 Gfx_WorldSpacePos : Gfx_WorldSpacePos,
	float4 MR_FragCoord : MR_FragCoord
)
{
	aLightOut = vec4(0);

@ifndef GFX_NO_SS_REFLECTIONS
	if (aUseReflection > 0.5 && aRoughness < 0.5)
	{
		float3 vertexToCamera = normalize(MR_GetPos(MR_ViewConstants.cameraToWorld).xyz - Gfx_WorldSpacePos);
		float3 reflected = normalize(reflect(-vertexToCamera, aNormal));

		aLightOut = SSR_GetReflection(SSR_FramebufferEarly, Gfx_WorldSpacePos, MR_FragCoord, reflected, aNormal, Gfx_Environment.screenSpaceReflectionQuality, 1.0, 1.0);
	}
@endif
}
