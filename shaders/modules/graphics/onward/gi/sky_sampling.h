import "shaders/modules/graphics/onward/common/samplers_sky.h"
import "shaders/modules/graphics/onward/sh/sh_funcs.h"

float4 OW_Sky_SampleProbe( in float3 aDirection )
{
	Gfx_SH3Coeffs shCosineLobe = Gfx_SH3EvalCosineLobe( aDirection );
/*
	float3 skyColor = Gfx_SH3Dot(
		SkyBuffer[0].mySH3CoeffsR,
		SkyBuffer[0].mySH3CoeffsG,
		SkyBuffer[0].mySH3CoeffsB,
		shCosineLobe );

	float skyVisibility = Gfx_SH3Dot( SkyBuffer[0].mySH3CoeffsA, shCosineLobe );
*/
/*
	float3 skyColor = Gfx_SH3Dot(
		SkyBufferConstants.mySH3CoeffsR,
		SkyBufferConstants.mySH3CoeffsG,
		SkyBufferConstants.mySH3CoeffsB,
		shCosineLobe );

	float skyVisibility = Gfx_SH3Dot( SkyBufferConstants.mySH3CoeffsA, shCosineLobe );
*/
	float3 skyColor = Gfx_SH3Dot(
		OW_GIConstants.mySkySH3CoeffsR,
		OW_GIConstants.mySkySH3CoeffsG,
		OW_GIConstants.mySkySH3CoeffsB,
		shCosineLobe );

	float skyVisibility = Gfx_SH3Dot( OW_GIConstants.mySkySH3CoeffsA, shCosineLobe );

	return max( float4( skyColor, min( skyVisibility, 1.0 ) ), vec4( 0 ) );
}
