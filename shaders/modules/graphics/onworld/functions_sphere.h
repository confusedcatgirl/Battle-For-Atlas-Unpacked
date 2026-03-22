// ALMOST ALL THOSE FUNCTIONS ASSUME A SPHERE AT (0,0,0)

// ray intersects sphere

// e = -b +/- sqrt( b^2 - c )
// 0 < x < y: two intersection points, rayP outside sphere
// x < 0 < y: two intersection points, rayP inside sphere. 
//            y is in rayDir positive direction and x in the negative direction
float2 Sphere_RayIntersect(in float3 rayP, in float3 rayDir, in float sphereRadius)
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

bool Sphere_RayIntersectTest(float2 intersectionResult)
{
    return intersectionResult.x < intersectionResult.y && intersectionResult.y > 0.0f;
}

// test two spheres at the same time...

float3 Sphere_GetClosestPointOnRay(in float3 rayOrigin, in float3 rayDir)
{
    const float3 toCenter = /*sphereCenter?*/ - rayOrigin;
    float3 pointOnRay = rayOrigin + rayDir * dot(toCenter, rayDir);
    return pointOnRay;
}

bool Sphere_GetClosestPointOnSphere(in float3 rayOrigin, in float3 rayDir, in float sphereRadius, out float3 pointOnSphere)
{
    pointOnSphere = Sphere_GetClosestPointOnRay(rayOrigin, rayDir) /*-sphereCenter?*/;
    const float len = length(pointOnSphere);
    pointOnSphere = pointOnSphere * sphereRadius / len /*+sphereCenter?*/;
    return len <= sphereRadius;
}

float3 Sphere_GetClosestPointOnSphere(in float3 rayOrigin, in float3 rayDir, in float sphereRadius)
{
    float3 pointOnSphere;
    Sphere_GetClosestPointOnSphere(rayOrigin, rayDir, sphereRadius, pointOnSphere);
    return pointOnSphere;
}


// http://stackoverflow.com/questions/2656899/mapping-a-sphere-to-a-cube
float3 Sphere_CubifyPoint(float3 s)
{
    float xx2 = s.x * s.x * 2.0f;
    float yy2 = s.y * s.y * 2.0f;

    float2 v = float2(xx2 - yy2, yy2 - xx2);

    float ii = v.y - 3.0f;
    ii *= ii;

    float isqrt = -sqrt(ii - 12.0f * xx2) + 3.0f;

    v = sqrt(v + isqrt);
    const float isqrt2 = 0.70710676908493042;
    v *= isqrt2;

    return sign(s) * float3(v, 1.0f);
}

// TODO optimize
float2 Sphere_PointToCube(float3 sphere)
{
    float3 f = abs(sphere);

    bool a = f.y >= f.x && f.y >= f.z;
    bool b = f.x >= f.z;

    float2 cubePos =  a ? Sphere_CubifyPoint(sphere.xzy).xy : b ? Sphere_CubifyPoint(sphere.yzx).xy : Sphere_CubifyPoint(sphere).xy;
    
    if (a)
    {
        cubePos.y = sphere.y < 0.0f? -cubePos.y : cubePos.y;
    }
    else if (b)
    {
        float temp = cubePos.x;
        cubePos.x = sphere.x < 0.0f? -cubePos.y : cubePos.y;
        cubePos.y = temp;
    }
    else
    {
        cubePos.x = sphere.z > 0.0f? -cubePos.x : cubePos.x;
    }

    return cubePos;
}