float FresnelSchlickFactor(float aDot)
{
	return pow(saturate(1.0 - aDot), 5.0);
}

float FresnelSchlickFactor(float3 aV1, float3 aV2)
{
	return FresnelSchlickFactor( dot(aV1, aV2) );
}

float FresnelSchlick(float aSpec, float3 aV1, float3 aV2)
{
	return aSpec + (1.0 - aSpec) * FresnelSchlickFactor(aV1, aV2);
}

float3 FresnelSchlick(float3 aSpec, float3 aV1, float3 aV2)
{
	return aSpec + (vec3(1.0) - aSpec) * FresnelSchlickFactor(aV1, aV2);
}
