// CubeMapFaceID, min3, max3 have a hardware implementation on PS4

float3 MR_CubeMapCoords(in float3 v)
{
	float3 result;

	result.x = CubeMapSCoord(v.x, v.y, v.z);
	result.y = CubeMapTCoord(v.x, v.y, v.z);

	result.z = CubeMapMajorAxis(v.x, v.y, v.z);

	result.z = rcp(abs(result.z));

	result.x = mad(result.x, result.z, 1.5);
	result.y = mad(result.y, result.z, 1.5);

	result.z = CubeMapFaceID(v.x, v.y, v.z);

	return result;
}
