
float4 OW_SampleEquirectangularTextureNoSeams(MR_Sampler2D aSampler, in float3 aDirection)
{
	float4 uv;
	uv.x = atan2( aDirection.z, aDirection.x );
	uv.x += MR_PI * 2.0 * step( uv.x, 0.0 );
	uv.yw = acos( aDirection.y );

	uv.z = atan2( aDirection.z, abs( aDirection.x ) );

	uv /= float4( MR_PI * 2.0, MR_PI, MR_PI * 2.0, MR_PI );
	return MR_SampleGrad( aSampler, uv.xy, ddx( uv.zw ), ddy( uv.zw ) );
}