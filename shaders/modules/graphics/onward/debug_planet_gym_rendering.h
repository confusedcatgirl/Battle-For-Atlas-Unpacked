import "shaders/modules/graphics/onward/functions/simplexnoise3d.h"

@ifdef GFX_FORWARD_SHADER

import "shaders/modules/graphics/onworld/gi_baking.h"

float3 OW_DEBUG_SampleBakingGI( in float3 aCameraPosition, in float3 aDirection )
{
	return OW_BakingGI_GetGI( aCameraPosition, aDirection, normalize( aCameraPosition ) );
}

float4 OW_DEBUG_SampleSkyColourLookup( in float3 aPosition )
{
	return MR_SampleLod0( SkyColourLookup, aPosition );
}

float4 OW_DEBUG_SampleFog2DLookup( in float2 aUV )
{
	return float4(0,0,0,1);
}

@else

float3 OW_DEBUG_SampleBakingGI( in float3 aCameraPosition, in float3 aDirection )
{
	return float3(0,0,0);
}

float4 OW_DEBUG_SampleSkyColourLookup( in float3 aPosition )
{
	return float4(0,0,0,1);
}

float4 OW_DEBUG_SampleFog2DLookup( in float2 aUV )
{
	return float4(0,0,0,1);
}

@endif // GFX_FORWARD_SHADER

float4 OW_DEBUG_SampleSkyCoronaLookup( in float3 aPosition )
{
	return float4(0,0,0,1);
}

float OW_DEBUG_FBM_5Octaves_3D( in float3 aCoord )
{
	float noise = SimplexNoise3D(aCoord);

	float3 coord2 = aCoord * float3(2, 2, 2);
	float SimplexNoise3D_2 = SimplexNoise3D(coord2);
	noise += SimplexNoise3D_2 * float(0.5);

	float3 coord3 = aCoord * float3(4, 4, 4);
	float SimplexNoise3D_3 = SimplexNoise3D(coord3);
	noise += SimplexNoise3D_3 * float(0.25);

	float3 coord4 = aCoord * float3(8, 8, 8);
	float SimplexNoise3D_4 = SimplexNoise3D(coord4);
	noise += SimplexNoise3D_4 * float(0.125);

	float3 coord5 = aCoord * float3(16, 16, 16);
	float SimplexNoise3D_5 = SimplexNoise3D(coord5);
	noise += SimplexNoise3D_5 * float(0.0625);

	noise = noise + float(1);
	noise = noise / float(2);

	return noise;
}