
float3 MR_CubeMapCoords(in float3 v)
{
	float3 result;

	result.x = __XB_Cube_SC(v.x, v.y, v.z);
	result.y = __XB_Cube_TC(v.x, v.y, v.z);

	result.z = __XB_Cube_MA(v.x, v.y, v.z);

	result.z = rcp(abs(result.z));

	result.x = mad(result.x, result.z, 1.5);
	result.y = mad(result.y, result.z, 1.5);

	result.z = __XB_Cube_ID(v.x, v.y, v.z);

	return result;
}
