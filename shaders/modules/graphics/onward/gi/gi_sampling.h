import "shaders/modules/graphics/onward/common/samplers_gi.h"
import "shaders/MRender/MR_ViewConstants.h"

float3 OW_GI_SampleFallback( in float4 aFallback[6], in float3 aDirection )
{
	float3 x = ( aDirection.x > 0 ? aFallback[ 0 ].rgb : aFallback[ 1 ].rgb );
	float3 y = ( aDirection.y > 0 ? aFallback[ 2 ].rgb : aFallback[ 3 ].rgb );
	float3 z = ( aDirection.z > 0 ? aFallback[ 4 ].rgb : aFallback[ 5 ].rgb );
	aDirection *= aDirection;
	return x * aDirection.x + y * aDirection.y + z * aDirection.z;
}

float3 OW_GI_SampleGalaxy_NOSTRENGTH( in float3 aPosition, in float3 aDirection )
{
	float3 pos = saturate( aPosition * OW_GIConstants.myGalaxy.myVolumeInvSize + OW_GIConstants.myGalaxy.myVolumeBias );

@ifdef OGI_MERGE_VOLUME_BASES
	float3 zzz = clamp( pos.zzz, OW_GIConstants.myGalaxy.myVolumeRange.zzz, OW_GIConstants.myGalaxy.myVolumeRange.www );
	zzz = zzz * 0.5f + step( 0.0f, aDirection ) * 0.5f;
	float3 x = MR_SampleLod0( GalaxyGIVolume_PX, float3( pos.xy, zzz.x ) ).rgb;
	float3 y = MR_SampleLod0( GalaxyGIVolume_NX, float3( pos.xy, zzz.y ) ).rgb;
	float3 z = MR_SampleLod0( GalaxyGIVolume_PY, float3( pos.xy, zzz.z ) ).rgb;
@else
	float3 x = ( aDirection.x > 0 ? MR_SampleLod0( GalaxyGIVolume_PX, pos ).rgb : MR_SampleLod0( GalaxyGIVolume_NX, pos ).rgb );
	float3 y = ( aDirection.y > 0 ? MR_SampleLod0( GalaxyGIVolume_PY, pos ).rgb : MR_SampleLod0( GalaxyGIVolume_NY, pos ).rgb );
	float3 z = ( aDirection.z > 0 ? MR_SampleLod0( GalaxyGIVolume_PZ, pos ).rgb : MR_SampleLod0( GalaxyGIVolume_NZ, pos ).rgb );
@endif // OGI_MERGE_VOLUME_BASES

	aDirection *= aDirection;

	return x * aDirection.x + y * aDirection.y + z * aDirection.z;
}

float3 OW_GI_SampleGalaxy( in float3 aPosition, in float3 aDirection )
{
	return OW_GI_SampleGalaxy_NOSTRENGTH( aPosition, aDirection ) * OW_GIConstants.myGalaxy.myStrength;
}

float3 OW_GI_SampleGalaxy( in float3 aPosition, in float3 aDirection, in float3 aFallback )
{
	float cameraDistance = distance( aPosition, MR_GetPos(MR_ViewConstants.cameraToWorld).xyz );
	float lerpFactor = smoothstep( OW_GIConstants.myGalaxy.myVolumeRange.x, OW_GIConstants.myGalaxy.myVolumeRange.y, cameraDistance );
	return lerp( OW_GI_SampleGalaxy( aPosition, aDirection ), aFallback, lerpFactor );
}

float3 OW_GI_TransformDirection( in float3 anUpVector, in float3 aNormal )
{
	float3 absUpVector = abs( anUpVector );

	float3 xAxis = float3( -anUpVector.z, 0, anUpVector.x );

	if (absUpVector.y > absUpVector.x && absUpVector.y > absUpVector.z)
	{
		xAxis = float3( -absUpVector.y, anUpVector.x * ( anUpVector.y >= 0.f ? 1.f : -1.f ), 0.f );
	}

	xAxis = normalize( xAxis );

	const float3 zAxis = cross( xAxis, anUpVector );

	float3 result;
	result.x = dot( aNormal, xAxis );
	result.y = dot( aNormal, anUpVector );
	result.z = dot( aNormal, zAxis );
	return result;
}

float3 OW_GI_SamplePlanet_NOSTRENGTH( in float3 aPosition, in float3 anUpVector, in float3 aDirection )
{
	float3 pos = saturate( aPosition * OW_GIConstants.myPlanet.myVolumeInvSize + OW_GIConstants.myPlanet.myVolumeBias );

	aDirection = OW_GI_TransformDirection( anUpVector, aDirection );

@ifdef OGI_MERGE_VOLUME_BASES
	float3 zzz = clamp( pos.zzz, OW_GIConstants.myPlanet.myVolumeRange.zzz, OW_GIConstants.myPlanet.myVolumeRange.www );
	zzz = zzz * 0.5f + step( 0.0f, aDirection ) * 0.5f;
	float3 x = MR_SampleLod0( PlanetGIVolume_PX, float3( pos.xy, zzz.x ) ).rgb;
	float3 y = MR_SampleLod0( PlanetGIVolume_NX, float3( pos.xy, zzz.y ) ).rgb;
	float3 z = MR_SampleLod0( PlanetGIVolume_PY, float3( pos.xy, zzz.z ) ).rgb;
@else
	float3 x = ( aDirection.x > 0 ? MR_SampleLod0( PlanetGIVolume_PX, pos ).rgb : MR_SampleLod0( PlanetGIVolume_NX, pos ).rgb );
	float3 y = ( aDirection.y > 0 ? MR_SampleLod0( PlanetGIVolume_PY, pos ).rgb : MR_SampleLod0( PlanetGIVolume_NY, pos ).rgb );
	float3 z = ( aDirection.z > 0 ? MR_SampleLod0( PlanetGIVolume_PZ, pos ).rgb : MR_SampleLod0( PlanetGIVolume_NZ, pos ).rgb );
@endif // OGI_MERGE_VOLUME_BASES

	aDirection *= aDirection;

	return x * aDirection.x + y * aDirection.y + z * aDirection.z;
}

float3 OW_GI_SamplePlanet_NOSTRENGTHFALLBACK( in float3 aPosition, in float3 anUpVector, in float3 aDirection, in float3 aFallback )
{
	float cameraDistance = distance( aPosition, MR_GetPos(MR_ViewConstants.cameraToWorld).xyz );
	float lerpFactor = smoothstep(OW_GIConstants.myPlanet.myVolumeRange.x, OW_GIConstants.myPlanet.myVolumeRange.y, cameraDistance );
	return lerp( OW_GI_SamplePlanet_NOSTRENGTH( aPosition, anUpVector, aDirection ), aFallback, lerpFactor ) * OW_GIConstants.myPlanet.myStrength;
}