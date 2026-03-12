struct SkewParameters
{
	float4 preTransform0;
	float4 preTransform1;
	float4 preTransform2;
	float2 corner;
	float2 rcpLengths;
	float2 remapPoint00;
	float2 remapPoint01;
	float2 remapPoint10;
	float2 remapPoint11;
	float4 remapNormal;
	float4 remapTangent;
};


// Compute barycentric coordinates of a point in a triangle with vertices on [0,0], [0,1], [1,1]
float3 CalculateBarycentricCoords(float2 aPoint)
{
	return float3(1.f - aPoint.y, aPoint.y - aPoint.x, aPoint.x);
}

//  Skewing is performed considering a quad as 2 triangles, then computing barycentric coordinates of
// the incoming point on the triangle it resides, and using those coordinates to map it to the 
// corresponding skewed triangle
void SkewPosition(SkewParameters skewParameters, inout float3 pos)
{
	float3 preTransformed;
	preTransformed[0] = dot(skewParameters.preTransform0, float4(pos, 1.0f));
	preTransformed[1] = dot(skewParameters.preTransform1, float4(pos, 1.0f));
	preTransformed[2] = dot(skewParameters.preTransform2, float4(pos, 1.0f));
	pos = preTransformed;
    
	float2 skewPos01         = (pos.xz - skewParameters.corner) * skewParameters.rcpLengths;
    bool   selectVertices    = skewPos01.x < skewPos01.y;
	float3 barycentricCoords = CalculateBarycentricCoords( selectVertices ? skewPos01 : skewPos01.yx );

    pos.xz = barycentricCoords.x * skewParameters.remapPoint00 +     
             barycentricCoords.y * (selectVertices ? skewParameters.remapPoint01 : skewParameters.remapPoint10) +
             barycentricCoords.z * skewParameters.remapPoint11;
}

void SkewNormal(SkewParameters skewParameters, inout float3 normal)
{
	float3 preTransformed;
	preTransformed[0] = dot(skewParameters.preTransform0.xyz, normal);
	preTransformed[1] = dot(skewParameters.preTransform1.xyz, normal);
	preTransformed[2] = dot(skewParameters.preTransform2.xyz, normal);
	normal.xz = normal.x*skewParameters.remapNormal.xy + normal.z*skewParameters.remapNormal.zw;
	normal = normalize(normal);
}

void SkewTangent(SkewParameters skewParameters, inout float3 tangent)
{
	float3 preTransformed;
	preTransformed[0] = dot(skewParameters.preTransform0.xyz, tangent);
	preTransformed[1] = dot(skewParameters.preTransform1.xyz, tangent);
	preTransformed[2] = dot(skewParameters.preTransform2.xyz, tangent);
	tangent.xz = tangent.x*skewParameters.remapTangent.xy + tangent.z*skewParameters.remapTangent.zw;
	tangent = normalize(tangent);
}
