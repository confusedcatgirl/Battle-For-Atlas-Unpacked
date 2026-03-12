import "shaders/modules/graphics/onworld/atmosphere/bruneton/common.h"
import "shaders/modules/graphics/onworld/env_constants.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  __  .___  ___. .______    __       _______ .___  ___.  _______ .__   __. .___________.    ___   .___________. __    ______   .__   __. //
// |  | |   \/   | |   _  \  |  |     |   ____||   \/   | |   ____||  \ |  | |           |   /   \  |           ||  |  /  __  \  |  \ |  | //
// |  | |  \  /  | |  |_)  | |  |     |  |__   |  \  /  | |  |__   |   \|  | `---|  |----`  /  ^  \ `---|  |----`|  | |  |  |  | |   \|  | //
// |  | |  |\/|  | |   ___/  |  |     |   __|  |  |\/|  | |   __|  |  . `  |     |  |      /  /_\  \    |  |     |  | |  |  |  | |  . `  | //
// |  | |  |  |  | |  |      |  `----.|  |____ |  |  |  | |  |____ |  |\   |     |  |     /  _____  \   |  |     |  | |  `--'  | |  |\   | //
// |__| |__|  |__| | _|      |_______||_______||__|  |__| |_______||__| \__|     |__|    /__/     \__\  |__|     |__|  \______/  |__| \__| //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float3 RemapPosition( in float3 aX, out float3 aD, out float aR)
{
	aR = MR_BrunetonAtmConstants.const_AtmosphereTransitionMapping;

	aD = normalize(aX);
	return aD * aR;
}

//inscattered light along ray x+tv, when sun in direction s (=S[L]-T(x,x0)S[L]|x0)
float3 Inscatter_Ground(inout float3 x, inout float t, float3 v, float3 s, inout float r, inout float mu, out float3 attenuation, float3 x0RealPosition) 
{
	attenuation = 0.0f;
	float3 result;
	float atmosphereRadius = Atm_GetRadius(OW_PlanetConstants.const_SkySphere.w);
	float d = -r * mu - sqrt(r * r * (mu * mu - 1.0) + atmosphereRadius * atmosphereRadius);

	{
		x += d * v;
		t -= d;
		mu = (r * mu + d) / MR_BrunetonAtmConstants.const_Rt;
		r = MR_BrunetonAtmConstants.const_Rt;
	}

	{
		float nu = dot(v, s);
		float muS = dot(x, s) / r;
		float phaseR = PhaseFunctionR(nu);
		float phaseM = PhaseFunctionM(nu);
		float4 inscatter = max(Texture4D(InscatterTexture, r, mu, muS, nu), 0.0);
		{
			float3 x0 = x0RealPosition;
			float r0 = length(x0);
			float rMu0 = dot(x0, v);
			float mu0 = rMu0 / r0;
			float muS0 = dot(x0, s) / r0;
			// avoids imprecision problems in transmittance computations based on textures
			attenuation = AnalyticTransmittance(r, mu, t);

			if (r0 > MR_BrunetonAtmConstants.const_Rg + 0.01)
			{
				inscatter = max(inscatter - attenuation.rgbr * Texture4D(InscatterTexture, r0, mu0, muS0, nu), 0.0);
			}
		}

		// avoids imprecision problems in Mie scattering when sun is below horizon
		inscatter.w *= smoothstep(0.00, 0.02, muS);

		result = max(inscatter.rgb * phaseR + Mie(inscatter) * phaseM, 0.0);
	}

	return result * MR_OnworldEnvironmentConstants.onPlanetSunLightIntensity;
}

//ground radiance at end of ray x+tv, when sun in direction s
//attenuated bewteen ground and viewer (=R[L0]+R[L*])
float3 GroundColor(float3 x0, float t, float3 v, float3 s, float r, float mu, float3 attenuation, float3 reflectance)
{
	float r0 = length(x0);
	float3 n = x0 / r0;

	// direct sun light (radiance) reaching x0
	float muS = dot(n, s);
	
	float3 sunLight = TransmittanceWithShadow(r0, muS);

	// precomputed sky light (irradiance) (=E[L*]) at x0
	float3 groundSkyLight = Irradiance(IrradianceTexture, r0, muS);
	float3 groundColor = reflectance * (sunLight + groundSkyLight) / Bruneton_Pi;

	return attenuation * groundColor; //=R[L0]+R[L*]
}

//inscattered light along ray x+tv, when sun in direction s (=S[L]-T(x,x0)S[L]|x0)
float3 Inscatter_Below(float3 x, float3 v, float3 s, float r, float mu) 
{
	float nu = dot(v, s);
	float muS = dot(x, s) / r;
	float4 inscatter = max(Texture4D(InscatterTexture, r, mu, muS, nu), 0.0);

	// avoids imprecision problems in Mie scattering when sun is below horizon
	inscatter.w *= smoothstep(0.00, 0.02, muS);

	float phaseR = PhaseFunctionR(nu);
	float phaseM = PhaseFunctionM(nu);
	return max(inscatter.rgb * phaseR + Mie(inscatter) * phaseM, 0.0) * MR_OnworldEnvironmentConstants.onPlanetSunLightIntensity;
}

//inscattered light along ray x+tv, when sun in direction s (=S[L]-T(x,x0)S[L]|x0)
float3 Inscatter_BelowHorizonClamped(float3 x, float3 v, float3 s, float r, float mu) 
{
	float nu = dot(v, s);
	float muS = max(dot(x, s) / r, 0.0f);
	float4 inscatter = max(Texture4D(InscatterTexture, r, mu, muS, nu), 0.0);

	// avoids imprecision problems in Mie scattering when sun is below horizon
	inscatter.w *= smoothstep(0.00, 0.02, muS);

	float phaseR = PhaseFunctionR(nu);
	float phaseM = PhaseFunctionM(nu);
	return max(inscatter.rgb * phaseR + Mie(inscatter) * phaseM, 0.0) * MR_OnworldEnvironmentConstants.onPlanetSunLightIntensity;
}

//inscattered light along ray x+tv, when sun in direction s (=S[L]-T(x,x0)S[L]|x0)
float3 InscatterMie_Below(float3 x, float3 v, float3 s, float r, float mu) 
{
	float nu = dot(v, s);
	float muS = dot(x, s) / r;
	float4 inscatter = max(Texture4D(InscatterTexture, r, mu, muS, nu), 0.0);

	// avoids imprecision problems in Mie scattering when sun is below horizon
	inscatter.w *= smoothstep(0.00, 0.02, muS);

	float phaseM = PhaseFunctionM(nu);
	return max(Mie(inscatter) * phaseM, 0.0) * MR_OnworldEnvironmentConstants.onPlanetSunLightIntensity;
}

// direct sun light for ray x+tv, when sun in direction s (=L0)
float3 SunColor(float3 v, float3 s, float r, float mu) 
{
	float3 transmittance = TransmittanceWithShadow(r, mu); // T(x,xo)
	float isun = step(cos(Bruneton_Pi / 180.0), dot(v, s)) * MR_OnworldEnvironmentConstants.onPlanetSunLightIntensity; // Lsun
	return transmittance * isun; // Eq (9)
}

float3 SunLight(float r, float mu)
{
	return TransmittanceWithShadow(r, mu) * MR_OnworldEnvironmentConstants.onPlanetSunLightIntensity;
}

float3 SunDomeColor(in float3 x, in float3 v, in float3 s)
{
	float4 xyzDir_wR;
	x = RemapPosition( x, xyzDir_wR.xyz, xyzDir_wR.w );
	const float r = xyzDir_wR.w;
	const float mu = dot(xyzDir_wR.xyz, v);
	return TransmittanceWithShadow( r, mu ) * MR_OnworldEnvironmentConstants.onPlanetSunLightIntensity;
}

float3 Inscatter_AtmosphereRim(inout float3 x, in float3 v, in float3 s, inout float r, inout float mu)
{
	float atmosphereRadius = MR_BrunetonAtmConstants.const_Rt; // use reference radius here since 'x' was remapped
	float d = -r * mu - sqrt(r * r * (mu * mu - 1.0) + atmosphereRadius * atmosphereRadius);

	{ // if x in space and ray intersects atmosphere
		// move x to nearest intersection of ray with top atmosphere boundary
		x += d * v;
		mu = (r * mu + d) / MR_BrunetonAtmConstants.const_Rt;
		r = MR_BrunetonAtmConstants.const_Rt;
	}

	float nu = dot(v, s);
	float muS = dot(x, s) / r;
	float phaseR = PhaseFunctionR(nu);
	float phaseM = PhaseFunctionM(nu);
	float4 inscatter = max(Texture4D(InscatterTexture, r, mu, muS, nu), 0.0);

	// avoids imprecision problems in Mie scattering when sun is below horizon
	inscatter.w *= smoothstep(0.00, 0.02, muS);

	return max(inscatter.rgb * phaseR + Mie(inscatter) * phaseM, 0.0);
}

float4 AtmosphereDomeColor(float3 x, float3 v, float3 s, float2 posClipSpace, out float r, out float mu)
{
	//remap height in atmosphere
	//to 6360km-6420km range that bruneton expects
	float maxAtmosphereRadius = OW_Planet_GetAtmosphereRadius();
	float terrainRadius = OW_Planet_GetMinTerrainRadius();
	float atmosphereHeight = maxAtmosphereRadius - terrainRadius;
	r = min(length(x), maxAtmosphereRadius);
	r = max( (r - terrainRadius) / (atmosphereHeight), 0.0f);

	r = ( r * 60.0f) + 6360.0f;
	
	r = max( r, 6360.0f);

	x = normalize(x);
	float3 x1 = x * r;

	float3 v1 = v;
	mu = dot( x, normalize(v));

	float3 inscatterColor = Inscatter_Below(x1, v, s, r, mu); //S[L]-T(x,xs)S[l]|xs
	float3 sunColor = vec3(0.0f);//SunColor(v1, s, r, mu); //L0

	return float4(inscatterColor, 1.0f);
}

float4 AtmosphereDomeColor(float3 x, float3 v, float3 s, float2 posClipSpace)
{
	float r, mu;
	return AtmosphereDomeColor( x, v, s, posClipSpace, r, mu );
}

float4 AtmosphereRimColor(float3 x, float3 v, float3 s, float3 i, float aRadius)
{
	x *= MR_BrunetonAtmConstants.const_Rt * rcp(aRadius); // scale view position to match reference radius... otherwise it doesn't work on small radius

	float r = length(x);
	float mu = dot(x, v) / r;

	float3 inscatterColor = Inscatter_AtmosphereRim(x, v, s, r, mu) * i; //S[L]-T(x,xs)S[l]|xs
	// float3 sunColor = SunColor(v, s, r, mu); //L0
	return float4(inscatterColor, 1.0f);
}

float4 AtmosphereRimColor(float3 x, float3 v, float3 s, float3 i)
{
	return AtmosphereRimColor(x, v, s, i, Atm_GetRadius(OW_PlanetConstants.const_SkySphere.w));
}

//always make sure to rotate view to keep atmosphere horizon at intersection of view and planet horizon
float3 RotateLookupViewByHeight( float3 v, float3 x)
{
	v = normalize(MR_Transform((float3x3)MR_BrunetonAtmConstants.const_HorizonRotation, v));
	return v;
}

float3 GetAtmosphereColor(in float3 x, in float3 v, in float3 s)
{
	float4 xyzDir_wR;
	v = RotateLookupViewByHeight(v, x);
	x = RemapPosition( x, xyzDir_wR.xyz, xyzDir_wR.w );
	return Inscatter_Below( x, v, s, xyzDir_wR.w, dot( xyzDir_wR.xyz, v ) );
}

float3 GetAtmosphereMieColor(in float3 x, in float3 v, in float3 s)
{
	float4 xyzDir_wR;
	x = RemapPosition( x, xyzDir_wR.xyz, xyzDir_wR.w );
	return InscatterMie_Below( x, v, s, xyzDir_wR.w, dot( xyzDir_wR.xyz, v ) );
}

float3 GetSunColor(in float3 x, in float3 v, in float3 s)
{
	float4 xyzDir_wR;
	x = RemapPosition( x, xyzDir_wR.xyz, xyzDir_wR.w );
	return TransmittanceWithShadow( xyzDir_wR.w, dot(xyzDir_wR.xyz, v));
}

float3 GetSunColorClamped(in float3 x, in float3 v, in float3 s, in float maxDot)
{
	float4 xyzDir_wR;
	x = RemapPosition( x, xyzDir_wR.xyz, xyzDir_wR.w );
	return TransmittanceWithShadow( xyzDir_wR.w, max(maxDot, dot(xyzDir_wR.xyz, v)));
}

float3 GetSunColor(in float3 x, in float3 s)
{
	return GetSunColor( x, s, s);
}

float3 GetSunColorClamped(in float3 x, in float3 s, in float maxDot)
{
	return GetSunColorClamped( x, s, s, maxDot);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// .______       _______  _______  _______ .______       _______ .__   __.   ______  _______  //
// |   _  \     |   ____||   ____||   ____||   _  \     |   ____||  \ |  |  /      ||   ____| //
// |  |_)  |    |  |__   |  |__   |  |__   |  |_)  |    |  |__   |   \|  | |  ,----'|  |__    //
// |      /     |   __|  |   __|  |   __|  |      /     |   __|  |  . `  | |  |     |   __|   //
// |  |\  \----.|  |____ |  |     |  |____ |  |\  \----.|  |____ |  |\   | |  `----.|  |____  //
// | _| `._____||_______||__|     |_______|| _| `._____||_______||__| \__|  \______||_______| //
////////////////////////////////////////////////////////////////////////////////////////////////

// //inscattered light along ray x+tv, when sun in direction s (=S[L]-T(x,x0)S[L]|x0)
// float3 Inscatter_REF(inout float3 x, inout float t, float3 v, float3 s, out float r, out float mu, out float3 attenuation) 
// {
//     float3 result;
//     r = length(x);
//     mu = dot(x, v) / r;
//     float d = -r * mu - sqrt(r * r * (mu * mu - 1.0) + MR_BrunetonAtmConstants.const_Rt * MR_BrunetonAtmConstants.const_Rt);
//     if (d > 0.0) { // if x in space and ray intersects atmosphere
//         // move x to nearest intersection of ray with top atmosphere boundary
//         x += d * v;
//         t -= d;
//         mu = (r * mu + d) / MR_BrunetonAtmConstants.const_Rt;
//         r = MR_BrunetonAtmConstants.const_Rt;
//     }
//     if (r <= MR_BrunetonAtmConstants.const_Rt) { // if ray intersects atmosphere
//         float nu = dot(v, s);
//         float muS = dot(x, s) / r;
//         float phaseR = PhaseFunctionR(nu);
//         float phaseM = PhaseFunctionM(nu);
//         float4 inscatter = max(Texture4D(InscatterTexture, r, mu, muS, nu), 0.0);
//         if (t > 0.0) {
//             float3 x0 = x + t * v;
//             float r0 = length(x0);
//             float rMu0 = dot(x0, v);
//             float mu0 = rMu0 / r0;
//             float muS0 = dot(x0, s) / r0;
//             // avoids imprecision problems in transmittance computations based on textures
//             attenuation = AnalyticTransmittance(r, mu, t);

//             if (r0 > MR_BrunetonAtmConstants.const_Rg + 0.01)
//             {
//                 // computes S[L]-T(x,x0)S[L]|x0
//                 inscatter = max(inscatter - attenuation.rgbr * Texture4D(InscatterTexture, r0, mu0, muS0, nu), 0.0);
//                 // avoids imprecision problems near horizon by interpolating between two points above and below horizon
//                 const float EPS = 0.004;
//                 float muHoriz = -sqrt(1.0 - (MR_BrunetonAtmConstants.const_Rg / r) * (MR_BrunetonAtmConstants.const_Rg / r));
//                 if (abs(mu - muHoriz) < EPS) {
//                     float a = ((mu - muHoriz) + EPS) / (2.0 * EPS);

//                     mu = muHoriz - EPS;
//                     r0 = sqrt(r * r + t * t + 2.0 * r * t * mu);
//                     mu0 = (r * mu + t) / r0;
//                     float4 inScatter0 = Texture4D(InscatterTexture, r, mu, muS, nu);
//                     float4 inScatter1 = Texture4D(InscatterTexture, r0, mu0, muS0, nu);
//                     float4 inScatterA = max(inScatter0 - attenuation.rgbr * inScatter1, 0.0);

//                     mu = muHoriz + EPS;
//                     r0 = sqrt(r * r + t * t + 2.0 * r * t * mu);
//                     mu0 = (r * mu + t) / r0;
//                     inScatter0 = Texture4D(InscatterTexture, r, mu, muS, nu);
//                     inScatter1 = Texture4D(InscatterTexture, r0, mu0, muS0, nu);
//                     float4 inScatterB = max(inScatter0 - attenuation.rgbr * inScatter1, 0.0);

//                     inscatter = lerp(inScatterA, inScatterB, a);
//                 }
//             }
//         }

//         // avoids imprecision problems in Mie scattering when sun is below horizon
//         inscatter.w *= smoothstep(0.00, 0.02, muS);

//         result = max(inscatter.rgb * phaseR + Mie(inscatter) * phaseM, 0.0);
//     } else { // x in space and ray looking in space
//         result = 0.0;
//     }
//     return result * MR_BrunetonAtmConstants.const_ISun;
// }

// //ground radiance at end of ray x+tv, when sun in direction s
// //attenuated bewteen ground and viewer (=R[L0]+R[L*])
// float3 GroundColor_REF(float3 x, float t, float3 v, float3 s, float r, float mu, float3 attenuation, float3 reflectance)
// {
//     float3 result;
//     if (t >= 0.0) 
//     { // if ray hits ground surface
//         // ground reflectance at end of ray, x0
//         float3 x0 = x + t * v;
//         float r0 = length(x0);
//         float3 n = x0 / r0;
//         // float2 coords = float2(atan(n.y, n.x), acos(n.z)) * float2(0.5, 1.0) / M_PI + float2(0.5, 0.0);
//         // float4 reflectance = 0.5f; //texture2D(reflectanceSampler, coords) * float4(0.2, 0.2, 0.2, 1.0);
//         // if (r0 > MR_BrunetonAtmConstants.const_Rg + 0.01) {
//         //     reflectance = float3(0.4, 0.4, 0.4);
//         // }

//         // direct sun light (radiance) reaching x0
//         float muS = dot(n, s);
//         //TransmittanceWithShadow(float r, float mu, float MR_BrunetonAtmConstants.const_Rg, float MR_BrunetonAtmConstants.const_Rt, MR_Sampler2D transmittanceTexture)
//         float3 sunLight = TransmittanceWithShadow(r0, muS);

//         // precomputed sky light (irradiance) (=E[L*]) at x0
//         float3 groundSkyLight = Irradiance(IrradianceTexture, r0, muS);

//         // light reflected at x0 (=(R[L0]+R[L*])/T(x,x0))
//         // float3 groundColor = reflectance * (max(muS, 0.0) * sunLight + groundSkyLight) * ISun / Bruneton_Pi;
//         // float3 groundColor = reflectance * (max(muS, 0.0) * sunLight + groundSkyLight) * ISun / Bruneton_Pi;
//         float3 groundColor = reflectance * (max(muS, 0.0) * sunLight + groundSkyLight) / Bruneton_Pi;

//         // // water specular color due to sunLight
//         // if (reflectance.w > 0.0) {
//         //     float3 h = normalize(s - v);
//         //     float fresnel = 0.02 + 0.98 * pow(1.0 - dot(-v, h), 5.0);
//         //     float waterBrdf = fresnel * pow(max(dot(h, n), 0.0), 150.0);
//         //     groundColor += reflectance.w * max(waterBrdf, 0.0) * sunLight * ISun;
//         // }

//         result = attenuation * groundColor; //=R[L0]+R[L*]
//     } 
//     else { // ray looking at the sky
//         result = float3(1.0f, 0.0f, 0.0);
//     }

//     return result;
// }

// // direct sun light for ray x+tv, when sun in direction s (=L0)
// float3 SunColor_REF(float3 x, float t, float3 v, float3 s, float r, float mu) 
// {
//     if (t > 0.0) {
//         return 0.0;
//     } else {
//         float3 transmittance = r <= MR_BrunetonAtmConstants.const_Rt ? TransmittanceWithShadow(r, mu) : 1.0; // T(x,xo)
//         float isun = step(cos(Bruneton_Pi / 180.0), dot(v, s)) * MR_BrunetonAtmConstants.const_ISun; // Lsun
//         return transmittance * isun; // Eq (9)
//     }
// }
