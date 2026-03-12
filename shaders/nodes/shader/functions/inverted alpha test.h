void InvertedAlphaTest(inout float3 color, in float alpha, float Gfx_AlphaRef : Gfx_AlphaRef0)
{
	if (alpha >= Gfx_AlphaRef) discard;
}