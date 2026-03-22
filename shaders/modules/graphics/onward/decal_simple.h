import "shaders/MRender/MR_ViewConstants.h"
import "shaders/MRender/MR_DepthBuffer.h"

import "shaders/modules/graphics/onworld/planet_flattening.h"

constants DecalEntity : MR_ConstantBuffer0
{
	int4 mySegment;

	float4x4 myTransform;
	float4x4 myInverseTransform;
	float4x4 myTextureTransform;

	float4 myColor;
}

float3 GetDecalUV(float4 fragCoord, out float4 color)
{
	float depth = MR_SampleLod0(Depthbuffer, MR_PixelToFramebufferUV(fragCoord.xy)).x;

	float3 unflattenedWorldSpace = MR_ClipToWorld(MR_PixelToClip(fragCoord.xy), depth);
	float validity = OW_SFT_ApplyUnflattening(unflattenedWorldSpace);
	color = DecalEntity.myColor * float4(1.0f, 1.0f, 1.0f, validity);

	float3 uv = MR_Transform(DecalEntity.myInverseTransform, unflattenedWorldSpace).xyz;
	float modelSpace = max(abs(uv.x), max(abs(uv.y), abs(uv.z)));
	if (modelSpace > 0.5)
		discard;

	return MR_Transform(DecalEntity.myTextureTransform, unflattenedWorldSpace).xyz;
}