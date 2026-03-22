float4 OW_SampleEquirectangularTextureLOD0(MR_Sampler2D aSampler, in float3 aDirection)
{
	float2 uv;
	uv.x = atan2( aDirection.z, aDirection.x );
	uv.x += MR_PI * 2.0 * step( uv.x, 0.0 );
	uv.y = acos( aDirection.y );
	uv.y += MR_PI * 2.0 * step( uv.y, 0.0 );

	uv /= float2( MR_PI * 2.0, MR_PI );
	return MR_SampleLod0( aSampler, uv );
}