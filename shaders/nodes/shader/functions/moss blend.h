
float AngleFactor( float3 normal, float3 direction, float angle, float angleFadeScale )
{
	const float AngleFadeScaleMax = 0.4;
	
	// The lower angle limit that the fade will go to
	float angleFadeLimit = angle - angleFadeScale * AngleFadeScaleMax;
	
	float d = dot( normal, direction );
	d = d * 0.5 + 0.5; // remap to [0,1]
	return smoothstep( angleFadeLimit, angle, d );
}

float HeightFunc( float heightValue, float heightThreshold, float thresholdDistance, float fadeScale, float scaleDirection )
{
	// Distance of the fade
	float fadeDistance = thresholdDistance * fadeScale;
	fadeDistance = max( fadeDistance, 0.001 ); // Clamp to avoid division by zero
	
	float scale = scaleDirection / fadeDistance;
	
	float offset = 1 - scale * heightThreshold;

	return heightValue * scale + offset;
}

float HeightFactor( float height, float heightTop, float heightBottom, float heightFadeScale )
{
	// We evaluate two functions (top and bottom) and take the minimum of them
	float topFunc = HeightFunc( height, heightTop, 1 - heightTop, heightFadeScale, -1 );
	float bottomFunc = HeightFunc( height, heightBottom, heightBottom, heightFadeScale, 1 );
	
	return saturate( min( topFunc, bottomFunc ) );
}

float NoiseFactor( float noise, float noiseThreshold, float noiseSharpness )
{
	// Offset from threshold to bounds, based on sharpness
	float offset = ( 1 - noiseSharpness ) * 0.5;
	
	float lowerBound = noiseThreshold - offset;
	float upperBound = noiseThreshold + offset;

	return smoothstep( lowerBound, upperBound, noise );
}

float3 AdjustMossColor( float3 mossColor, float3 normal, float3 toCameraDirection, float rimLight, float fadeDarken, float blendValue )
{
	float3 adjustedColor = mossColor;

	// RimLight
	{
		// Dot, clamp and invert - gives a value between 1 (at the edge) and 0 (in the center)
		float d = 1 - saturate( dot( normal, toCameraDirection ) );
		
		// Sharpen the effect
		d = d * d;
		
		adjustedColor *= d * rimLight + 1; // +1 since the effect is additive
	}
	
	// Fade Darkening
	{
		// Apply the darkening setting by inverting and multiplying, saturate is needed since 'fadeDarken' can be bigger than 1
		float darken = 1 - saturate( ( 1 - blendValue ) * fadeDarken );
		
		adjustedColor *= darken;
	}

	return adjustedColor;
}

void MossBlend(

	// Blend Inputs
	float3 textureColor,
	float3 textureColor2,
	float3 tangentNormal,
	float3 tangentNormal2,
	float ambientOcclusion,
	float ambientOcclusion2,
	float roughness,
	float roughness2,
	float metalness,
	float metalness2,
	
	// Blend Parameters
	float noise,
	float noiseThreshold,
	float noiseSharpness,
	float height,
	float heightTop,
	float heightBottom,
	float heightFadeScale,
	float3 worldNormal,
	float3 direction,
	float angle,
	float angleFadeScale,
	float fadeDarkening,
	float rimLightAmount,
	float3 toCameraDirection,
	float alphaMask,
	float alphaMaskScale,
	float unscaledAlphaMask,
	
	// Blend Outputs
	out float3 outputColor,
	out float3 outputNormal,
	out float outputAmbientOcclusion,
	out float outputRoughness,
	out float outputMetalness
	
	)
{
	// Get all factors affecting the blend
	float angleFactor = AngleFactor( worldNormal, direction, angle, angleFadeScale );
	float heightFactor = HeightFactor( height, heightTop, heightBottom, heightFadeScale );
	float noiseFactor = NoiseFactor( noise, noiseThreshold, noiseSharpness );
	
	// Use the minimum of the factors
	float finalLerpValue = min( angleFactor, min( heightFactor, noiseFactor ) );
	finalLerpValue *= lerp( 1, alphaMask, alphaMaskScale ); // apply the moss alpha mask
	finalLerpValue *= unscaledAlphaMask;
	finalLerpValue = saturate( finalLerpValue );
	
	// Modify the moss color
	float3 modifiedMossColor = AdjustMossColor( textureColor2, worldNormal, toCameraDirection, rimLightAmount, fadeDarkening, finalLerpValue );
	
	// Final lerp
	outputColor = lerp( textureColor, modifiedMossColor, finalLerpValue );
	outputNormal = lerp( tangentNormal, tangentNormal2, finalLerpValue );
	outputAmbientOcclusion = lerp( ambientOcclusion, ambientOcclusion2, finalLerpValue );
	outputRoughness = lerp( roughness, roughness2, finalLerpValue );
	outputMetalness = lerp( metalness, metalness2, finalLerpValue );
}
