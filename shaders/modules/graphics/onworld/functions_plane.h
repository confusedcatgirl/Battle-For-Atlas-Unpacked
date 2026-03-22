float Plane_RayIntersect(in float3 rayOrigin, in float3 rayDir, in float3 planeNormal, in float3 planePoint)
{
    float denominator = dot(planeNormal, rayDir);

    return denominator == 0.0f? -1.0f : dot(planeNormal, (planePoint - rayOrigin)) / denominator;
}

bool Plane_RayIntersectTest(float intersectionResult)
{
    return intersectionResult >= 0.0f;
}

bool Plane_SegmentIntersectTest(float intersectionResult)
{
    return intersectionResult >= 0.0f && intersectionResult <= 1;
}

// bool Sphere_RayIntersectTest(float2 intersectionResult)
// {
//     return intersectionResult.x < intersectionResult.y;
// }

// // float3 Sphere_RayIntersectFOO(in float3 rayP, in float3 rayDir, in float sphereRadius)
// // {
// //     float b = dot( rayP, rayDir );
// //     float c = dot( rayP, rayP ) - sphereRadius * sphereRadius;

// //     float d = b * b - c;
// //     if ( d < 0.0 )
// //     {
// //         return float3( 1.0f, -1.0f, d);
// //     }
// //     d = sqrt( d );
// //     return float3( -b - d, -b + d, 0.0f);
// // }


// // test two spheres at the same time...


// float3 Sphere_GetClosestPointOnRay(in float3 rayOrigin, in float3 rayDir)
// {
//     const float3 toCenter = /*sphereCenter?*/ - rayOrigin;
//     float3 pointOnRay = rayOrigin + rayDir * dot(toCenter, rayDir);
//     return pointOnRay;
// }

// bool Sphere_GetClosestPointOnSphere(in float3 rayOrigin, in float3 rayDir, in float sphereRadius, out float3 pointOnSphere)
// {
//     pointOnSphere = Sphere_GetClosestPointOnRay(rayOrigin, rayDir) /*-sphereCenter?*/;
//     const float len = length(pointOnSphere);
//     pointOnSphere = pointOnSphere * sphereRadius / len /*+sphereCenter?*/;
//     return len <= sphereRadius;
// }

// float3 Sphere_GetClosestPointOnSphere(in float3 rayOrigin, in float3 rayDir, in float sphereRadius)
// {
//     float3 pointOnSphere;
//     Sphere_GetClosestPointOnSphere(rayOrigin, rayDir, sphereRadius, pointOnSphere);
//     return pointOnSphere;
// }