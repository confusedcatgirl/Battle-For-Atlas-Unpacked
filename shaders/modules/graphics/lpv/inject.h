MR_Sampler2D ReflectedLight : MR_Texture0 { anisotropy = 0, wrap = border, border = 0 0 0 0, filter = point };
MR_Sampler2D Depth : MR_Texture1 { anisotropy = 0, wrap = border, border = 0 0 0 0, filter = point };
MR_Sampler2D DepthNormal : MR_Texture2 { anisotropy = 0, wrap = clamp, filter = point };

constants : MR_ConstantBuffer0
{
	float4x4 SrcToLPV;
	float4x4 CamToLPV;
	float4x4 CamToWorld;
	float4x4 CamToClip;
	float4x4 ClipToCam;
	float3 LightOffset;
	float2 SrcUVOffset;
	float2 SrcUVScale;
	float2 ViewportSize;
	float Resolution;
	float Scale;
	float NormalOffset;
}
