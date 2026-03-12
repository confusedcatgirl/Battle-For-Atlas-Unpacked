
pixel float2 UIGetBlitEffectFullscreenUV(float4 aClipPos : Gfx_ClipPosition)
{
	return aClipPos.xy * float2(0.5, -0.5) + float2(0.5, 0.5);
}
