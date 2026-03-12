
float3 MR_CubeMapCoords(in float3 v)
{
	float4 coords; // (uv, face, major axis)

	if (abs(v.z) >= abs(v.x) && abs(v.z) >= abs(v.y))
	{
		coords = (v.z < 0.0) ?
			float4(-v.x,-v.y,5.0,-v.z):
			float4( v.x,-v.y,4.0, v.z);
	}
	else if (abs(v.y) >= abs(v.x))
	{
		coords = (v.y < 0.0) ?
			float4(v.x,-v.z,3.0,-v.y):
			float4(v.x, v.z,2.0, v.y);
	}
	else
	{
		coords = (v.x < 0.0) ?
			float4( v.z,-v.y,1.0,-v.x):
			float4(-v.z,-v.y,0.0, v.x);
	}

	coords.xy = coords.xy / (2.0f * coords.w) + 1.5f;

	return coords.xyz;
}

