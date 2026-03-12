


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

float AngleFactor( float d, float angle, float angleFadeScale )
{
    const float AngleFadeScaleMax = 0.4;
    // The lower angle limit that the fade will go to
    float angleFadeLimit = angle - angleFadeScale * AngleFadeScaleMax;
    return smoothstep( angleFadeLimit, angle, d );
}


void PlanetLayerWeight(
	
	float height <default = 0.5>,
	float heightTop <default = 1>,
	float heightBottom <default = 0>,
	float heightFadeScale <default = 1>,

    float vertexAngle <default = 1>,
    float angle <default = 1>,
    float angleFadeScale <default = 0.5>,

	// Blend Outputs
	out float weight
	
	)
{

    float angleFactor = AngleFactor( vertexAngle, angle, angleFadeScale );
	weight = min(angleFactor, HeightFactor( height, heightTop, heightBottom, heightFadeScale ));
}
