import "shaders/MRender/MR_ViewConstants.h"

@ifdef USE_BRANCHLESS

void OW_Utils_BranchlessONB(in float3 n, out float3 b1, out float3 b2)
{
	float signZ = (n.z >= 0.0f ? 1.0f : -1.0f);
	const float a = -1.0f / (signZ + n.z);
	const float b = n.x * n.y * a;
	b1 = float3(1.0f + signZ * n.x * n.x * a, signZ * b, -signZ * n.x);
	b2 = float3(b, signZ + n.y * n.y * a, -n.y);
}

float3x3 OW_Foliage_CreateTransform(in float3 aUp, in float4 aQuat)
{
	float3 x, z;
	OW_Utils_BranchlessONB(aUp, x, z);

	return MR_ColumnMatrix(x, aUp, z);
}

@else

import "shaders/modules/graphics/onward/common/foliage_constants.h"

float3x3 OW_Foliage_CreateTransform(in float3 aUp, in float4 aQuat)
{
	float3 tanZ = normalize( cross( ForceFieldConstants.constBaseAxis.xyz, aUp ) );
	float3 tanX = cross( aUp, tanZ );
	return MR_ColumnMatrix( tanX, aUp, tanZ );
}

@endif // USE_BRANCHLESS

vertex
{
	struct TransfromOutput
	{
		float3 params;
		float3 forces;
	};
}

pixel
{
@ifdef JITTER
	import "shaders/nodes/Shader/Functions/Dither.h"
@else
	import "shaders/modules/graphics/onward/dithering.h"
@endif // JITTER

	constants : MR_ConstantBuffer2
	{
		float4 constDithering1;
		float4 constDithering2;
	}

	void OW_Foliage_Dither(in float aDistanceFromCamera, in float2 vpos)
	{
		float ditherI = mad(aDistanceFromCamera, constDithering1.x, constDithering1.y);
		float ditherO = mad(aDistanceFromCamera, constDithering1.w, constDithering1.z);

@ifdef JITTER
		const float randValue = TAANoise( Gfx_Environment.noiseFrame, vpos, 3.0 );
@else
		const float randValue = OW_Noise_Hash2To1NoSin( vpos );
@endif // JITTER

		clip( randValue - ditherO );
		clip( ditherI - randValue );
	}

	void OW_Foliage_Dither(in float3 worldPosition, in float2 vpos)
	{
		OW_Foliage_Dither(distance(worldPosition, MR_ViewConstants.LODCameraPosition), vpos);
	}
}
