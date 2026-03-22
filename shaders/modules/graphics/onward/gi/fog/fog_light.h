
float3 OW_FogLight_GetSunLightPhase( in float3 aToSun, in float3 aViewDir, in float anAnisotropy )
{
	const float g = dot( aToSun, aViewDir );

	float lobe0 = OW_FogLight_GetPhaseFunction( g, anAnisotropy );
	float lobe1 = OW_FogLight_GetPhaseFunction( g, anAnisotropy * -0.5 );

	// use 1/3 of the energy as a wide backward scattering
	return lerp( lobe0, lobe1, 1.0 / 3.0 );
}
