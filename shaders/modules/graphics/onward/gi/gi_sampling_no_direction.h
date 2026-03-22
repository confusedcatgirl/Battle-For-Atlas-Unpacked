import "shaders/modules/graphics/onward/common/samplers_gi.h"
import "shaders/MRender/MR_ViewConstants.h"

float3 OW_GI_SampleFallback_NoDirection( in float4 aFallback[6] )
{
	float3 color = vec3(0.0);
	color += aFallback[0].rgb;
	color += aFallback[1].rgb;
	color += aFallback[2].rgb;
	color += aFallback[3].rgb;
	color += aFallback[4].rgb;
	color += aFallback[5].rgb;
	return color / 6.0;
}

float3 OW_GI_SampleGalaxy_NoDirection_NOSTRENGTH( in float3 aPosition )
{
	float3 pos = saturate( aPosition * OW_GIConstants.myGalaxy.myVolumeInvSize + OW_GIConstants.myGalaxy.myVolumeBias );

	float3 color = vec3(0.0);
@ifdef OGI_MERGE_VOLUME_BASES
	float3 zzz = clamp( pos.zzz, OW_GIConstants.myGalaxy.myVolumeRange.zzz, OW_GIConstants.myGalaxy.myVolumeRange.www ) * vec3(0.5);
	color += MR_SampleLod0( GalaxyGIVolume_PX, float3( pos.xy, zzz.x ) ).rgb;
	color += MR_SampleLod0( GalaxyGIVolume_NX, float3( pos.xy, zzz.y ) ).rgb;
	color += MR_SampleLod0( GalaxyGIVolume_PY, float3( pos.xy, zzz.z ) ).rgb;
	zzz += vec3(0.5);
	color += MR_SampleLod0( GalaxyGIVolume_PX, float3( pos.xy, zzz.x ) ).rgb;
	color += MR_SampleLod0( GalaxyGIVolume_NX, float3( pos.xy, zzz.y ) ).rgb;
	color += MR_SampleLod0( GalaxyGIVolume_PY, float3( pos.xy, zzz.z ) ).rgb;
@else
	color += MR_SampleLod0( GalaxyGIVolume_PX, pos ).rgb;
	color += MR_SampleLod0( GalaxyGIVolume_NX, pos ).rgb;
	color += MR_SampleLod0( GalaxyGIVolume_PY, pos ).rgb;
	color += MR_SampleLod0( GalaxyGIVolume_NY, pos ).rgb;
	color += MR_SampleLod0( GalaxyGIVolume_PZ, pos ).rgb;
	color += MR_SampleLod0( GalaxyGIVolume_NZ, pos ).rgb;
@endif // OGI_MERGE_VOLUME_BASES

	return color / 6.0;
}

float3 OW_GI_SampleGalaxy_NoDirection( in float3 aPosition, in float3 aFallback )
{
	float cameraDistance = distance( aPosition, MR_GetPos(MR_ViewConstants.cameraToWorld).xyz );
	float lerpFactor = smoothstep( OW_GIConstants.myGalaxy.myVolumeRange.x, OW_GIConstants.myGalaxy.myVolumeRange.y, cameraDistance );
	return lerp( OW_GI_SampleGalaxy_NoDirection_NOSTRENGTH( aPosition ) * OW_GIConstants.myGalaxy.myStrength, aFallback, lerpFactor );
}

float3 OW_GI_SamplePlanet_NoDirection_NOSTRENGTH( in float3 aPosition )
{
	float3 pos = saturate( aPosition * OW_GIConstants.myPlanet.myVolumeInvSize + OW_GIConstants.myPlanet.myVolumeBias );

	float3 color = vec3(0.0);
@ifdef OGI_MERGE_VOLUME_BASES
	float3 zzz = clamp( pos.zzz, OW_GIConstants.myPlanet.myVolumeRange.zzz, OW_GIConstants.myPlanet.myVolumeRange.www ) * vec3(0.5);
	color += MR_SampleLod0( PlanetGIVolume_PX, float3( pos.xy, zzz.x ) ).rgb;
	color += MR_SampleLod0( PlanetGIVolume_NX, float3( pos.xy, zzz.y ) ).rgb;
	color += MR_SampleLod0( PlanetGIVolume_PY, float3( pos.xy, zzz.z ) ).rgb;
	zzz += vec3(0.5);
	color += MR_SampleLod0( PlanetGIVolume_PX, float3( pos.xy, zzz.x ) ).rgb;
	color += MR_SampleLod0( PlanetGIVolume_NX, float3( pos.xy, zzz.y ) ).rgb;
	color += MR_SampleLod0( PlanetGIVolume_PY, float3( pos.xy, zzz.z ) ).rgb;
@else
	color += MR_SampleLod0( PlanetGIVolume_PX, pos ).rgb;
	color += MR_SampleLod0( PlanetGIVolume_NX, pos ).rgb;
	color += MR_SampleLod0( PlanetGIVolume_PY, pos ).rgb;
	color += MR_SampleLod0( PlanetGIVolume_NY, pos ).rgb;
	color += MR_SampleLod0( PlanetGIVolume_PZ, pos ).rgb;
	color += MR_SampleLod0( PlanetGIVolume_NZ, pos ).rgb;
@endif // OGI_MERGE_VOLUME_BASES

	return color / 6.0;
}

float3 OW_GI_SamplePlanet_NoDirection( in float3 aPosition, in float3 aFallback )
{
	float cameraDistance = distance( aPosition, MR_GetPos(MR_ViewConstants.cameraToWorld).xyz );
	float lerpFactor = smoothstep( OW_GIConstants.myPlanet.myVolumeRange.x, OW_GIConstants.myPlanet.myVolumeRange.y, cameraDistance );
	return lerp( OW_GI_SamplePlanet_NoDirection_NOSTRENGTH( aPosition ) * OW_GIConstants.myPlanet.myStrength, aFallback, lerpFactor );
}