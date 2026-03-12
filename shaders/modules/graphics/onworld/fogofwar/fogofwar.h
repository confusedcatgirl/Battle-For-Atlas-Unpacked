uint3 TransformDispatchCoordsToPixelCoords(uint3 dispatchThread, uint height)
{
    return uint3(dispatchThread.x, (height - 1) - dispatchThread.y, dispatchThread.z);
}

float3 TransformUVFacePosXToUnitSpherePos(float2 uv)
{
    float4x4 uvtolocal = {0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, -2.0f, 1.0f, 0.0f,
                        -2.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f};

    return normalize(MR_Transform(uvtolocal, float4(uv, 1.0f, 1.0f)).xyz);
}

//     // -X
// {
//     MC_Vector4f x(0.0f, 0.0f, 2.0f, 0.0f);
//     MC_Vector4f y(0.0f, -2.0f, 0.0f, 0.0f);
//     MC_Vector4f z(-1.0f, 1.0f, -1.0f, 0.0f);
//     myFaceToLocal[1] = MC_Matrix44(x, y, z, lastVector);
// }
float3 TransformUVFaceNegXToUnitSpherePos(float2 uv)
{
    float4x4 uvtolocal = {0.0f, 0.0f, -1.0f, 0.0f,
                        0.0f, -2.0f, 1.0f, 0.0f,
                        2.0f, 0.0f, -1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f};

    return normalize(MR_Transform(uvtolocal, float4(uv, 1.0f, 1.0f)).xyz);
}

// // +Y
// {
//     MC_Vector4f x(2.0f, 0.0f, 0.0f, 0.0f);
//     MC_Vector4f y(0.0f, 0.0f, 2.0f, 0.0f);
//     MC_Vector4f z(-1.0f, 1.0f, -1.0f, 0.0f);
//     myFaceToLocal[2] = MC_Matrix44(x, y, z, lastVector);
// }
float3 TransformUVFacePosYToUnitSpherePos(float2 uv)
{
    float4x4 uvtolocal = {2.0f, 0.0f, -1.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 2.0f, -1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f};

    return normalize(MR_Transform(uvtolocal, float4(uv, 1.0f, 1.0f)).xyz);
}

// // -Y
// {
//     MC_Vector4f x(2.0f, 0.0f, 0.0f, 0.0f);
//     MC_Vector4f y(0.0f, 0.0f, -2.0f, 0.0f);
//     MC_Vector4f z(-1.0f, -1.0f, 1.0f, 0.0f);
//     myFaceToLocal[3] = MC_Matrix44(x, y, z, lastVector);
// }
float3 TransformUVFaceNegYToUnitSpherePos(float2 uv)
{
    float4x4 uvtolocal = {2.0f, 0.0f, -1.0f, 0.0f,
                        0.0f, 0.0f, -1.0f, 0.0f,
                        0.0f, -2.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f};

    return normalize(MR_Transform(uvtolocal, float4(uv, 1.0f, 1.0f)).xyz);
}

// // +Z
// {
//     MC_Vector4f x(2.0f, 0.0f, 0.0f, 0.0f);
//     MC_Vector4f y(0.0f, -2.0f, 0.0f, 0.0f);
//     MC_Vector4f z(-1.0f, 1.0f, 1.0f, 0.0f);
//     myFaceToLocal[4] = MC_Matrix44(x, y, z, lastVector);
// }
float3 TransformUVFacePosZToUnitSpherePos(float2 uv)
{
    float4x4 uvtolocal = {2.0f, 0.0f, -1.0f, 0.0f,
                        0.0f, -2.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f};

    return normalize(MR_Transform(uvtolocal, float4(uv, 1.0f, 1.0f)).xyz);
}

// // -Z
// {
//     MC_Vector4f x(-2.0f, 0.0f, 0.0f, 0.0f);
//     MC_Vector4f y(0.0f, -2.0f, 0.0f, 0.0f);
//     MC_Vector4f z(1.0f, 1.0f, -1.0f, 0.0f);
//     myFaceToLocal[5] = MC_Matrix44(x, y, z, lastVector);
// }
float3 TransformUVFaceNegZToUnitSpherePos(float2 uv)
{
    float4x4 uvtolocal = {-2.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, -2.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, -1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f};

    return normalize(MR_Transform(uvtolocal, float4(uv, 1.0f, 1.0f)).xyz);
}
