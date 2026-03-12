
float3 Gfx_EncodeNormal_UNorm(float3 normal)
{
	const float scale = 511.0 / 1023.0;
	return normal * scale + scale;
}

float3 Gfx_DecodeNormal_UNorm(float3 encodedNormalUNorm)
{
	return encodedNormalUNorm * (1023.0 / 511.0) - 1.0;
}

float3 Gfx_DecodeNormal_UNorm_Normalized(float3 encodedNormalUNorm)
{
	return normalize(encodedNormalUNorm - 511.0 / 1023.0);
}
