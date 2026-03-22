
float2 DirectionToLatLongUV(float3 aNormal)
{
	float Pi = 3.1415926535897932384626433832795;
	float2 ret;
	ret.x = (atan2(aNormal.x, aNormal.z) - Pi * 0.5) / (2 * Pi);
	ret.y = (asin(aNormal.y) + Pi * 0.5) / Pi;
	return ret;
}
