
float2 OW_GetFogOfWar( MR_SamplerCube aTexture1, MR_SamplerCube aTexture2, in float3 aPosition, in float2 aSmoothing <default = 0.5 0.5> )
{
	float2 xy;
#ifdef IS_DX11
	xy.x = MR_SampleLod0(aTexture1, aPosition).x;
	xy.y = MR_SampleLod0(aTexture2, aPosition).x;
#else
	xy = MR_SampleLod0(aTexture1, aPosition).xy;
#endif // IS_DX11
	return smoothstep( vec2(0.5) - saturate(aSmoothing) * vec2(0.5),
	                   vec2(0.5) + saturate(aSmoothing) * vec2(0.5), xy);
}
