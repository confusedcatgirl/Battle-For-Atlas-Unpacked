float2 CartesianToSphericalCoords(in float3 aDirection)
{
	float kPI = 3.14159265359;
	float k2PI = 2.0f * kPI;
	float2 output;
	output.x = atan2( aDirection.z, aDirection.x);
	if (output.x < 0.0f) output.x += k2PI;
	output.y = acos( aDirection.y );
	
	output.x /= k2PI;
	output.y /= kPI;
	
	return output;
}