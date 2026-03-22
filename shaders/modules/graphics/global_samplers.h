
MR_Sampler2D SSAO : MR_SSAO;


MR_Sampler3D<float3> GIVolume	: MR_GIDirections  		{ filter = linear point, wrap = clamp };
MR_Sampler3D<float3> GIFallback	: MR_GIFallbackTexture	{ filter = linear point, wrap = clamp };