import "shaders/modules/graphics/environment_constants.h"
import "shaders/modules/graphics/lights.h"
import "shaders/modules/graphics/shadows.h"

@ifndef GFX_NO_SHADOWS

MR_Sampler2DArrayCmp DownsampledShadowMap : MR_Texture3
{
	filter = linear point,
	wrap = clamp,
	compare = >
};

constants : MR_ConstantBuffer0
{
	float2	OutputSize;
	float2	DepthScale;
	int		NumSamples;
	float	EndDistance;
	uint    ShadowResolution;
	uint	NumDepthSlices;
}

float GetShadow( float4 aShadowPos )
{
	float shadow_opaque = MR_SampleLod0Cmp(DownsampledShadowMap, aShadowPos.xyz, aShadowPos.w).x;
	return shadow_opaque;
}

float GetShadowMultipleTaps( float4 aShadowPos )
{
	float shadow = 0;

	int2 offsets[3] =
	{
		int2( -1, -1 ),
		int2( 0, 0 ),
		int2( 1, 1 ),
	};
	for( int i = 0; i < 3; ++i )
	{
		shadow += MR_SampleLod0CmpOffset( DownsampledShadowMap, aShadowPos.xyz, aShadowPos.w, offsets[i] ).x;
	}
	shadow /= 3;

	return shadow;
}

float CalculateSliceSteps( float3 slicePos, float3 sliceDir, float3 sliceMin, float3 sliceMax )
{
	float3 t;
	if( sliceDir.x > 0 )
	{
		t.x = ( sliceMax.x - slicePos.x ) / sliceDir.x;
	}
	else if( sliceDir.x < 0 )
	{
		t.x = ( sliceMin.x - slicePos.x ) / sliceDir.x;
	}
	else
	{
		t.x = 9999999;
	}
	if( sliceDir.y > 0 )
	{
		t.y = ( sliceMax.y - slicePos.y ) / sliceDir.y;
	}
	else if( sliceDir.y < 0 )
	{
		t.y = ( sliceMin.y - slicePos.y ) / sliceDir.y;
	}
	else
	{
		t.y = 9999999;
	}

	if( sliceDir.z > 0 )
	{
		t.z = ( sliceMax.z - slicePos.z ) / sliceDir.z;
	}
	else if( sliceDir.z < 0 )
	{
		t.z = ( sliceMin.z - slicePos.z ) / sliceDir.z;
	}
	else
	{
		t.z = 9999999;
	}

	return min( t.x, min(t.y, t.z) );
}

void RayMarch3D( int numSamples, MR_RWTexture3D< float > outputTexture, uint2 outputCoordinate, float maxDepth )
{
	float3 worldPosMax = MR_ClipToWorld((outputCoordinate + 0.5) * float2(2,-2) / OutputSize + float2(-1,1), maxDepth );

	float3 rayDirMax = worldPosMax - MR_ViewConstants.LODCameraPosition;
	float  worldPosTMax = length(rayDirMax);
	float3 rayDir = normalize(rayDirMax);

	float rayNumSamples = numSamples;
	float rayStartT = 0;
	float rayEndT = EndDistance;
	float rayStep = 1.0 / rayNumSamples;
	float3 rayStartPos = MR_ViewConstants.LODCameraPosition + rayDir * rayStartT;
	float3 rayEndPos = MR_ViewConstants.LODCameraPosition + rayDir * rayEndT;

	const float  epsilon = 0.0025;
	const float3 min_slice = float3(0.0 + epsilon, 0.0 + epsilon, 0.0 + epsilon);
	const float3 max_slice = float3(1.0 - epsilon, 1.0 - epsilon, 1.0 - epsilon);

	float intensity = 0;
	
	float4 shadowPos[4] =
	{
		MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[0], rayStartPos),
		MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[1], rayStartPos),
		MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[2], rayStartPos),
		MR_Transform(ShadowConstants.shadowMapWorldToClipZBiased[3], rayStartPos),
	};

	float3 dir = ( rayEndPos - rayStartPos ) / rayNumSamples;
	float4 shadowDir[4] =
	{
		 MR_Transform((float4x3) ShadowConstants.shadowMapWorldToClipZBiased[0], dir),
		 MR_Transform((float4x3) ShadowConstants.shadowMapWorldToClipZBiased[1], dir),
		 MR_Transform((float4x3) ShadowConstants.shadowMapWorldToClipZBiased[2], dir),
		 MR_Transform((float4x3) ShadowConstants.shadowMapWorldToClipZBiased[3], dir),
	};
	
	// Compute number of steps for each slice
	float t_0 = CalculateSliceSteps( shadowPos[0].xyw, shadowDir[0].xyw, min_slice, max_slice );
	float t_1 = CalculateSliceSteps( shadowPos[1].xyw, shadowDir[1].xyw, min_slice, max_slice );
	float t_2 = CalculateSliceSteps( shadowPos[2].xyw, shadowDir[2].xyw, min_slice, max_slice );
	float t_3 = CalculateSliceSteps( shadowPos[3].xyw, shadowDir[3].xyw, min_slice, max_slice );

	float sampleTInc = ( rayEndT - rayStartT ) * rayStep;
	
	float stepsToBreak = floor( ceil( ( worldPosTMax - rayStartT ) / sampleTInc ) - 1 );

	t_3 = min( t_3, min( rayNumSamples - 1, stepsToBreak ) );
	t_2 = min( t_2, t_3 );
	t_1 = min( t_1, t_2 );
	t_0 = min( t_0, t_1 );

	float stepsPerDepthSlice = rayNumSamples / float( NumDepthSlices );

	float i = 0;
	
	uint depthSlice = 0;
	float finalSliceStep = stepsPerDepthSlice - 1;
	float numSamplesThisSlice = 0;

	for( ; i <= t_0; i+=1 )
	{
		if( i > finalSliceStep )
		{
			outputTexture[ uint3( outputCoordinate.xy, depthSlice ) ] = ( intensity / max(1.0, numSamplesThisSlice) );
			intensity = 0;
			numSamplesThisSlice = 0;
			++depthSlice;
			if( depthSlice >= NumDepthSlices )
			{
				return;
			}
			finalSliceStep += stepsPerDepthSlice;
		}

		float inShadow = GetShadow( shadowPos[0] + shadowDir[0] * i );

		intensity += inShadow;
		numSamplesThisSlice += 1;
	}
	for( ; i <= t_1; i+=1 )
	{
		if( i > finalSliceStep )
		{
			outputTexture[ uint3( outputCoordinate.xy, depthSlice ) ] = ( intensity / max(1.0, numSamplesThisSlice) );
			intensity = 0;
			numSamplesThisSlice = 0;
			++depthSlice;
			if( depthSlice >= NumDepthSlices )
			{
				return;
			}
			finalSliceStep += stepsPerDepthSlice;
		}

		float inShadow = GetShadow( shadowPos[1] + shadowDir[1] * i );

		intensity += inShadow;
		numSamplesThisSlice+=1;
	}
	for( ; i <= t_2; i+=1 )
	{
		if( i > finalSliceStep )
		{
			outputTexture[ uint3( outputCoordinate.xy, depthSlice ) ] = ( intensity / max(1.0, numSamplesThisSlice) );
			intensity = 0;
			numSamplesThisSlice = 0;
			++depthSlice;
			if( depthSlice >= NumDepthSlices )
			{
				return;
			}
			finalSliceStep += stepsPerDepthSlice;
		}

		float inShadow = GetShadow( shadowPos[2] + shadowDir[2] * i );

		intensity += inShadow;
		numSamplesThisSlice+=1;
	}
	for( ; i <= t_3; i+=1 )
	{
		if( i > finalSliceStep )
		{
			outputTexture[ uint3( outputCoordinate.xy, depthSlice ) ] = ( intensity / max(1.0, numSamplesThisSlice) );
			intensity = 0;
			numSamplesThisSlice = 0;
			++depthSlice;
			if( depthSlice >= NumDepthSlices )
			{
				return;
			}
			finalSliceStep += stepsPerDepthSlice;
		}

		float inShadow = GetShadow( shadowPos[3] + shadowDir[3] * i );

		intensity += inShadow;
		numSamplesThisSlice+=1;
	}

	for( uint s = depthSlice; s < NumDepthSlices; ++s )
	{
		outputTexture[ uint3( outputCoordinate.xy, s ) ] = ( intensity / max(1.0, numSamplesThisSlice) );
	}
}

@endif
