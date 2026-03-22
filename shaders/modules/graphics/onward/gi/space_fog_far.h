import "shaders/nodes/Shader/Functions/Onward/Lighting/OW_SpaceGI.h"
import "shaders/modules/graphics/onward/gi/space_fog.h"

float3 OW_SpaceFog_GetFarPosition( in float3 aViewDir )
{
	return MR_GetPos( MR_ViewConstants.cameraToWorld ).xyz + aViewDir * OW_FogConstantBuffer.myGalaxy.myDistributionRange;
}

float3 OW_SpaceFog_GetFarLighting( in float3 aPosition, in float3 aViewDir )
{
	float3 radiance = OW_SpaceFog_GetSunLight( aPosition, aViewDir, OW_FogConstantBuffer.myGalaxy.myAnisotropy, 1.0 );
	return ( OW_GI_SampleFallback( OW_GIConstants.myGalaxy.myFallback, aViewDir ) + radiance );
}

float3 OW_SpaceFog_GetFarFogRawColor( in float3 aPosition, in float3 aViewDir )
{
	return OW_SpaceFog_GetFarLighting( aPosition, aViewDir ) * OW_FogConstantBuffer.myGalaxy.myReflectance;
}

float OW_SpaceFog_GetFarRawExtinction( )
{
	float extinction = dot( vec2( 0.5 ), OW_FogConstantBuffer.myGalaxy.myScatteringAbsorption.xy + OW_FogConstantBuffer.myGalaxy.myScatteringAbsorption.zw );
	extinction *= OW_FogConstantBuffer.myGalaxy.myDistributionRange;
	return extinction;
}

float3 OW_SpaceFog_GetFarColor( in float3 aPosition, in float3 aViewDir, in float anExtinction )
{
	const float3 color = OW_SpaceFog_GetFarFogRawColor( aPosition, aViewDir );

	float scattering = dot( vec2( 0.5 ), OW_FogConstantBuffer.myGalaxy.myScatteringAbsorption.xy );
	scattering *= OW_FogConstantBuffer.myGalaxy.myDistributionRange;

	return color * scattering; // we need to do something with absorption here, test with constants lighting
}

float3 OW_SpaceFog_GetFarColor( in float3 aPosition, in float3 aViewDir )
{
	return OW_SpaceFog_GetFarColor( aPosition, aViewDir, OW_SpaceFog_GetFarRawExtinction( ) );
}

float OW_SpaceFog_GetFarExtinction( )
{
	return exp2( -OW_SpaceFog_GetFarRawExtinction( ) );
}

float4 OW_SpaceFog_GetFar( in float3 aViewDir )
{
	return float4( OW_SpaceFog_GetFarColor( OW_SpaceFog_GetFarPosition( aViewDir ), aViewDir ), OW_SpaceFog_GetFarExtinction( ) );
}
