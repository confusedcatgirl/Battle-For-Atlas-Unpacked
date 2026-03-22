void FrustumCheck( in float3 aPosition, in float aSize )
{
	float4 particleSize = vec4(-aSize);

	float4 side;
	side.w = dot( aPosition, MR_ViewConstants.topPlane.xyz    ) + MR_ViewConstants.topPlane.w;
	side.z = dot( aPosition, MR_ViewConstants.bottomPlane.xyz ) + MR_ViewConstants.bottomPlane.w;
	side.x = dot( aPosition, MR_ViewConstants.rightPlane.xyz  ) + MR_ViewConstants.rightPlane.w;
	side.y = dot( aPosition, MR_ViewConstants.leftPlane.xyz   ) + MR_ViewConstants.leftPlane.w;

	return (any(side < particleSize) || (dot( aPosition, MR_ViewConstants.nearPlane.xyz ) < -MR_ViewConstants.nearPlane.w));
}