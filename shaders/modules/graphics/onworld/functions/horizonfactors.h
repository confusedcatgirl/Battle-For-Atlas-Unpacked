import "shaders/math/fastmath.h"

float4 OW_HorizonFactors_Relative( in float3 aEyePos, in float3 aEyeDir, in float aRadius, in float4 aShift, in float4 aDecay )
{
	float invCamHeight = rsqrt( dot( aEyePos, aEyePos ) );

	float horizon = asinFast4( aRadius * invCamHeight );
	const float3 v = aEyePos * invCamHeight;
	float angle = acosFast4( dot(v, -aEyeDir) );

	return saturate( exp( aDecay * (aShift+horizon.xxxx-angle.xxxx) ) );
}