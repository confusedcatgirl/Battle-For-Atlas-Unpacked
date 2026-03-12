import "shaders/modules/graphics/onward/ow_view_constants.h"

constants MR_ViewConstants : MR_ViewConstants
{
	float4x4 worldToCamera;
	float4x4 cameraToClip;
	float4x4 worldToClip;

	float4x4 cameraToWorld;
	float4x4 clipToCamera;
	float4x4 clipToWorld;

	float4x4 lastFrameWorldToClip;
	float4x4 AFRlastFrameWorldToClip;

	float4x4 unjitteredWorldToCamera;

	float4 nearPlane;
	float4 rightPlane;
	float4 leftPlane;
	float4 bottomPlane;
	float4 topPlane;
	float4 farPlane;
	float near;
	float far;

	float3 LODCameraPosition;
	float LODScale;

	float4 LODDistancePlane;

	float2 logicalViewportPos;
	float2 logicalViewportSize;

	float2 viewportPos;
	float2 viewportSize;

	float2 framebufferSize;
	float2 framebufferUVMin;

	float2 framebufferUVMax;
	float LODDistanceBias;
	int isShadowView; // not used yet

	float2 pixelToClipScale;
	float2 pixelToClipOffset;

	float2 clipToPixelScale;
	float2 clipToPixelOffset;

	float2 pixelToFramebufferUVScale;
	float2 pixelToFramebufferUVOffset;

	float2 clipToFramebufferUVScale;
	float2 clipToFramebufferUVOffset;

	float2 clipToQuarterFramebufferUVScale;
	float2 clipToQuarterFramebufferUVOffset;

	int2 viewportMin;
	int2 viewportMax;

	int4 basisSegment;
}

float2 MR_PixelToClip(float2 framebufferPixelCoord)
{
	return framebufferPixelCoord * MR_ViewConstants.pixelToClipScale + MR_ViewConstants.pixelToClipOffset;
}

float2 MR_ClipToPixel(float2 clipPos)
{
	return (clipPos - MR_ViewConstants.pixelToClipOffset) / MR_ViewConstants.pixelToClipScale;
}

float2 MR_FramebufferUVToPixel(float2 framebufferUVCoord)
{
	return (framebufferUVCoord - MR_ViewConstants.pixelToFramebufferUVOffset) / MR_ViewConstants.pixelToFramebufferUVScale;
}

float2 MR_PixelToFramebufferUV(float2 framebufferPixelCoord)
{
	return framebufferPixelCoord * MR_ViewConstants.pixelToFramebufferUVScale + MR_ViewConstants.pixelToFramebufferUVOffset;
}

float2 MR_ClipToFramebufferUV(float2 clipPos)
{
	return clipPos * MR_ViewConstants.clipToFramebufferUVScale + MR_ViewConstants.clipToFramebufferUVOffset;
}

float2 MR_ClipToQuarterFramebufferUV(float2 clipPos)
{
	return clipPos * MR_ViewConstants.clipToQuarterFramebufferUVScale + MR_ViewConstants.clipToQuarterFramebufferUVOffset;
}

float3 MR_ClipToCamera(float2 clipCoord2, float depth)
{
	float4 clip4;
	clip4.xy = clipCoord2;
	clip4.zw = MR_GetZAxis(MR_ViewConstants.cameraToClip).zw * depth + MR_GetPos(MR_ViewConstants.cameraToClip).zw;
	clip4.z /= clip4.w;
	clip4.w = 1;

	float4 cam = MR_Transform(MR_ViewConstants.clipToCamera, clip4);
	cam /= cam.w;
	return cam.xyz;
}

float3 MR_ClipToWorld(float2 clipCoord2, float depth)
{
	float3 cam = MR_ClipToCamera(clipCoord2, depth);
	return MR_Transform(MR_ViewConstants.cameraToWorld, cam).xyz;
}

float3 MR_PixelToCamera(float2 framebufferPixelCoord)
{
	float3 pos = MR_ClipToWorld(MR_PixelToClip(framebufferPixelCoord), 1);
	return normalize(MR_GetPos(MR_ViewConstants.cameraToWorld).xyz - pos);
}

float MR_ZToLinearDepth(float z)
{
	float2 t = MR_GetZAxis(MR_ViewConstants.clipToCamera).zw * z + MR_GetPos(MR_ViewConstants.clipToCamera).zw;
	return t.x / t.y;
}

// REMOVE THIS (USED IN CODE)
void OW_Transform_ClipZ(inout float4 clipPos)
{
}
