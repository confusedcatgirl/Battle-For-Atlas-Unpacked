float3 UnpackNormal(float4 packedNormal)
{
	float4 unpackColor = packedNormal * 2.0 - (2.0 * 128.0 / 255.0);
	float3 unpacked;
	unpacked.x = unpackColor.a;
	unpacked.y = unpackColor.g;
	unpacked.z = sqrt(saturate(1.0 - dot(unpackColor.ga, unpackColor.ga)));
	return unpacked;
}

void UnpackNormalAndAO(float4 packedNormal, out float3 normal, out float ambientOcclusion)
{
@ifdef GFX_DEBUG_NO_NORMALMAP
	normal = float3(0.0, 0.0, 1.0);
	ambientOcclusion = 1.0;
@else
	normal = UnpackNormal(packedNormal);
	ambientOcclusion = packedNormal.b;
@endif
}
