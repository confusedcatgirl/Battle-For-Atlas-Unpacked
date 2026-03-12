/////////////////////////
///// AUX FUNCTIONS ////
////////////////////////
float vmax(float3 v)
{
	return max(max(v.x, v.y), v.z);
}

float3 hsv2rgb(float3 c)
{
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, saturate(p - K.xxx), c.y);
}

////////////////////////
// DISTANCE FUNCTIONS //
////////////////////////
float DistanceToPlane(float3 position, float3 normal, float distance)
{
	return dot(position, normal) + distance;
}

float DistanceToBox(float3 position, float3 center, float3 box)
{
	return vmax(abs(position - center) - box);
}

float DistanceToSphere(float3 position, float3 center, float radius)
{
	return length(position - center) - radius;
}

float DistanceToCylinder(float3 postion, float3 c)
{
	return length(postion.xz - c.xy) - c.z;
}

float DistanceToTorusXY(float3 position, float2 t)
{
	float2 q = float2(length(position.xy) - t.x, position.z);
	return length(q) - t.y;
}

float DistanceToTorusXZ(float3 position, float2 t)
{
	float2 q = float2(length(position.xz) - t.x, position.y);
	return length(q) - t.y;
}

float DistanceToTorusYZ(float3 position, float2 t)
{
	float2 q = float2(length(position.yz) - t.x, position.x);
	return length(q) - t.y;
}

float DistancesIntersection(float d1, float d2)
{
	return max(d1, d2);
}

float DistancesUnion(float d1, float d2)
{
	return min(d1, d2);
}

