import "shaders/modules/graphics/environment_constants.h"

float4 OW_Foliage_SmoothCurve( float4 x )
{
	return x * x *( 3.0f - 2.0f * x );
}

float4 OW_Foliage_TriangleWave( float4 x )
{
	return abs( frac( x + 0.5f ) * 2.0f - 1.0f );
}

float4 OW_Foliage_SmoothWave( float4 x )
{
	return OW_Foliage_SmoothCurve( OW_Foliage_TriangleWave( x ) );
} 

float3 OW_Foliage_GetWindWaves( in float3 anObjectPos, in float3 aVertexPos )
{
	float objPhase = dot(anObjectPos.xyz, vec3(1.0f));

	float2 wavesIn = Gfx_Environment.time * float2(0.1f, 0.5f) + float2( objPhase + dot(aVertexPos.xyz, vec3(0.05f)), objPhase * 0.01f );

	float4 waves = (frac( wavesIn.xxyy * float4( 1.975f, 0.93f, 0.75f, 0.19f ) ) * 2.0f - 1.0f ) * float4( 0.32f, 0.036f, 0.03f, 0.23f );
	waves = OW_Foliage_SmoothWave( waves );
	return float3( (( waves.x + waves.y ) * 2.0f - 1.0f) * 0.5f, (waves.zw * 2.0f - 1.0f) );
}

float3 OW_Foliage_GetWindForce( in float3 aWindWaves, in float3 aDirection )
{
	return aDirection * (1.0f + aWindWaves.x);
}

float3 OW_Foliage_GetWindDirection( in float3 aWindWaves, in float3x3 aTransform )
{
	return MR_GetXAxis( aTransform ) * aWindWaves.y +  MR_GetZAxis( aTransform ) * aWindWaves.z;
}

float3 OW_Foliage_GetWindForce( in float3 anObjectPos, in float3 aVertexPos, in float3x3 aTransform )
{
	const float3 waves = OW_Foliage_GetWindWaves( anObjectPos, aVertexPos );
	return OW_Foliage_GetWindForce( waves, OW_Foliage_GetWindDirection( waves, aTransform ) );
}