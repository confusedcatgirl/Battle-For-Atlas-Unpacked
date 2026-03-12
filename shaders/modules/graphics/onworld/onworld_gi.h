import "shaders/modules/graphics/global_samplers.h"
import "shaders/modules/graphics/onworld/planet_base.h"

float3 OW_GI_Basis_GetDirection(in int idx)
{
	float3 basisDirections [] =
	{
		float3(  1.0,  0.0,  0.0 ),
		float3(  0.0,  1.0,  0.0 ),
		float3(  0.0,  0.0,  1.0 ),
		float3( -1.0,  0.0,  0.0 ),
		float3(  0.0, -1.0,  0.0 ),
		float3(  0.0,  0.0, -1.0 )
	};
	return basisDirections[idx];
}

float3 OW_GI_Basis_Sample(in float3 colors[6], in float3 normal)
{
	float3 basisDirections [] =
	{
		float3(  1.0,  0.0,  0.0 ),
		float3(  0.0,  1.0,  0.0 ),
		float3(  0.0,  0.0,  1.0 ),
		float3( -1.0,  0.0,  0.0 ),
		float3(  0.0, -1.0,  0.0 ),
		float3(  0.0,  0.0, -1.0 )
	};

	float totalWeight = 0;
	float3 totalColor = float3(0,0,0);

	for( uint i = 0; i < 6; ++i )
	{
		float d = saturate(dot( basisDirections[i], normal ));
		d = d * d;

		float weight = max( 0.00001, d ); // this max will ensure that if the normal is 0 the result will be the average of all directions
		totalWeight += weight;
		totalColor += colors[i] * weight;
	}

	totalColor /= max( 0.0001, totalWeight );
	return max( 0, totalColor );
}

void GI_FarAmbient(inout float3 directions[6], float3 worldPosition, float factor, float3 farAmbient)
{
    if (factor < 1.0f)
    {
        directions[0] = lerp( farAmbient, directions[0].rgb, factor );
        directions[1] = lerp( farAmbient, directions[1].rgb, factor );
        directions[2] = lerp( farAmbient, directions[2].rgb, factor );
        directions[3] = lerp( farAmbient, directions[3].rgb, factor );
        directions[4] = lerp( farAmbient, directions[4].rgb, factor );
        directions[5] = lerp( farAmbient, directions[5].rgb, factor );
    }
}

void GI_BakedProbe(inout float3 directions[6], float3 worldPosition, float factor)
{
    if (factor < 1.0f)
    {
        float3 uv = Gfx_Environment.GITextureCoordScale * worldPosition + Gfx_Environment.GITextureCoordBias;

		/* TODO
        directions[0] = lerp( MR_SampleLod0( GIVolume_Outdoor_X,  uv ).rgb, directions[0], factor );
        directions[1] = lerp( MR_SampleLod0( GIVolume_Outdoor_Y,  uv ).rgb, directions[1], factor );
        directions[2] = lerp( MR_SampleLod0( GIVolume_Outdoor_Z,  uv ).rgb, directions[2], factor );
        directions[3] = lerp( MR_SampleLod0( GIVolume_Outdoor_NX, uv ).rgb, directions[3], factor );
        directions[4] = lerp( MR_SampleLod0( GIVolume_Outdoor_NY, uv ).rgb, directions[4], factor );
        directions[5] = lerp( MR_SampleLod0( GIVolume_Outdoor_NZ, uv ).rgb, directions[5], factor );
		*/
    }
}
