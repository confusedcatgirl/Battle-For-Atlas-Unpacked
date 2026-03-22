
float3 GetReflectionDirection(float3 aIncoming, float3 aNormal, float aRoughness)
{
	// (very) approximate dominant direction
	float d = -dot(aIncoming, aNormal);
	float t = max(d, aRoughness);
	return aIncoming + (d + t) * aNormal;
}
