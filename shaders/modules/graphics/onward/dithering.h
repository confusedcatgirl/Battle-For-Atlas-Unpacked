import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/onworld/noise/functions_noise.h"

float OW_Dithering_GetGlobalFactor(in float aDistance, in int aObjectCategory)
{
	float3 dither = OW_ViewConstants.myParametersPerCategory[ aObjectCategory ].xyz;
	return saturate( saturate( aDistance * dither.x + dither.y) - dither.z );
}

float OW_Dithering_GetGlobalFactor(in float3 aWorldPosition, in int aObjectCategory)
{
	const float dist = abs( dot( aWorldPosition, MR_ViewConstants.LODDistancePlane.xyz ) + MR_ViewConstants.LODDistancePlane.w );
	return OW_Dithering_GetGlobalFactor( dist, aObjectCategory );
}

void OW_Dithering_CheckAndDiscardFromValue(in float randValue, in float factor)
{
@ifndef OGFX_NO_DITHERING
	clip( randValue - factor );
@endif // OGFX_NO_DITHERING
}

float OW_Dithering_CheckAndDiscard(in float2 vpos, in float factor)
{
@ifndef OGFX_NO_DITHERING
	float randValue = OW_Noise_Hash2To1NoSin( vpos );
	OW_Dithering_CheckAndDiscardFromValue( randValue, factor );
	return randValue;
@else
	return 1.0;
@endif // OGFX_NO_DITHERING
}

float OW_Dithering_CheckAndReturn(in float2 vpos, in float factor)
{
	if (factor <= 0.0f)
	{
		return 0.0f;
	}
	return step( OW_Noise_Hash2To1NoSin( vpos ), factor );
}