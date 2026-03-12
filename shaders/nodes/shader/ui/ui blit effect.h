
pixel float4 GetBlitEffectColor(float4 aUI, float4 aWorld)
{
	return float4(aUI.xyz + aWorld.xyz * aUI.a, 1);
}
