//float3 SphericalToCartesianCoords( float2 spherical, float distance )
//{
//	float aAzimuth = spherical.x;
//	float aZenith = spherical.y;
//	
//	float3 result = float3( 0.0f, 0.0f, 0.0f );
//
//	float kPI = 3.14159265359;
//	float piOver2 = kPI * 0.5f;
//
//	float sinOfPhiPlusPI2 = sin( aZenith + piOver2 );
//
//	result.x = distance * sin( aAzimuth ) * sinOfPhiPlusPI2;
//	result.z = distance * cos( aAzimuth ) * sinOfPhiPlusPI2;
//	result.y = distance * cos( aZenith + piOver2 );
//
//	return result;
//}


float3 SphericalToCartesianCoords(float2 cartesian, float distance )
{
	float kPI = 3.14159265359;
	float k2PI = 2.0f * kPI;
	float phi = cartesian.y * kPI;
	float theta = cartesian.x * k2PI;
	float sinphi = sin(phi);
	return float3(cos(theta) * sinphi, cos(phi), sin(theta) * sinphi) * distance;
} 