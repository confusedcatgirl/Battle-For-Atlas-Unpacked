
float3 OW_BRDF_GetDiffuseTerm( in float3 N, in float3 L )
{
	return saturate( dot(N, L) ) / MR_PI;
}

float3 OW_BRDF_GetDiffuseTerm( in float3 N, in float3 L, in float aOpacity )
{
	float diffuse = dot(N, L);
	diffuse = diffuse > 0.0f ? diffuse * aOpacity : saturate(-diffuse) * (1.0f - aOpacity);
	return diffuse / MR_PI;
}