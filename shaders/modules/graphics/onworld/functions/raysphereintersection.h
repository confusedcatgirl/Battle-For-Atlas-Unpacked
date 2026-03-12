// ALMOST ALL THOSE FUNCTIONS ASSUME A SPHERE AT (0,0,0)

// ray intersects sphere

// e = -b +/- sqrt( b^2 - c )
// 0 < x < y: two intersection points, rayP outside sphere
// x < 0 < y: two intersection points, rayP inside sphere. 
//            y is in rayDir positive direction and x in the negative direction
float2 OW_RaySphereIntersection(in float3 rayP, in float3 rayDir, in float sphereRadius)
{
    float b = dot( rayP, rayDir );
    float c = dot( rayP, rayP ) - sphereRadius * sphereRadius;

    float d = b * b - c;
    if ( d < 0.0 )
    {
        return float2( 1.0f, -1.0f );
    }
    d = sqrt( d );
    return float2( -b - d, -b + d );
}

bool OW_RaySphereIntersectionTest(float2 intersectionResult)
{
    return intersectionResult.x < intersectionResult.y;
}

float2 OW_RaySphereIntersection(in float3 rayP, in float3 rayDir, in float sphereRadius, in float3 sphereP)
{
    float3 centerToRay = rayP - sphereP;
    float b = dot( rayDir, centerToRay);
    float c = dot( centerToRay, centerToRay ) - sphereRadius * sphereRadius;

    float d = b * b - c;
    if ( d < 0.0 )
    {
        return float2( 1.0f, -1.0f );
    }
    d = sqrt( d );
    return float2( -b - d, -b + d );
}