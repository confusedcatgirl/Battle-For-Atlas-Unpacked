import "shaders/modules/graphics/onward/gi/volumetric_fog.h"
import "shaders/modules/graphics/onward/gi/volumetric_fog_constantbuffer.h"

const float OW_FOG_REPROJECTION_ALPHA = 0.85;

OW_VolumetricFog3DCoords OW_VolumetricFog_GetReprojectionCoords( in float3 aWorldPos, in float aDepth, out float aLastFrameAlpha )
{
	float4 position = MR_Transform( OW_FogConstantBuffer.myLastFrameMatrix, aWorldPos );
	position = MR_Transform( MR_ViewConstants.cameraToClip, position / position.w );
	float2 clipPos = position.xy / position.w;

	aLastFrameAlpha = any(abs(clipPos) >= vec2(1)) ? 0.0 : OW_FOG_REPROJECTION_ALPHA;

	return OW_VolumetricFog_GetCoordsCP( clipPos, aDepth );
}

float3 OW_FogVolume_GetCameraPos( )
{
	return MR_GetPos( MR_ViewConstants.cameraToWorld ).xyz;
}

float2 OW_FogVolume_GetUVFromThreadID( in uint3 aDispatchThreadId, in float3 aJitter, in float3 aInvSize )
{
	return ( float2( aDispatchThreadId.xy ) + vec2( 0.5 ) + aJitter.xy ) * aInvSize.xy;
}

float OW_FogVolume_GetRangeFromThreadID( in uint3 aDispatchThreadId, in float3 aJitter, in float3 aInvSize )
{
	float time = ( float( aDispatchThreadId.z ) + 0.5 + aJitter.z ) * aInvSize.z;
	return saturate( time );
}

float2 OW_FogVolume_GetClipFromUV( in float2 aUV )
{
	return aUV * float2( 2,-2 ) + float2( -1, 1 );
}

float3 OW_FogVolume_GetCameraPosToNearDir( in float3 aCameraPos, in float2 aUV )
{
	const float3 camNearPos = MR_ClipToCamera( OW_FogVolume_GetClipFromUV( aUV ), 1 );
	float3 worldNearPos = MR_Transform( MR_ViewConstants.cameraToWorld, camNearPos ).xyz;
	return normalize( worldNearPos - aCameraPos);
}
