MR_Sampler2D TransmittanceTexture : OW_BrunetonTextures0 { filter = linear, wrap = clamp };
MR_Sampler3D InscatterTexture     : OW_BrunetonTextures1 { filter = linear, wrap = clamp };
MR_Sampler2D IrradianceTexture    : OW_BrunetonTextures2 { filter = linear, wrap = clamp };

const float Bruneton_Pi = 3.1415926535897932384626433832795;

constants MR_BrunetonAtmConstants : OW_BrunetonConstants
{
	int const_ResR;
	int const_ResMU;
	int const_ResMUS;
	int const_ResNU;
	float const_Rg;
	float const_Rt;
	float const_RL;
	float const_HR;
	float3 const_BetaR;

	float const_HM;
	float3 const_BetaMSca;
	float const_MieG;
    float4x4 const_HorizonRotation;
    float const_AtmosphereTransitionMapping;
}

float ModGLSL(float x, float y)
{
	if(x >= 0.0f)
	{
		return fmod(x, y);
	}

	float result = fmod(-x, y);
	return (result == 0.0)? result : y - result;
}

uint3 GetPixelCoords(uint3 dispatchThread, uint height)
{
	return uint3(dispatchThread.x, (height - 1) - dispatchThread.y, dispatchThread.z);
}

void MuMuSNu(float pixelX, float pixelY, float r, float4 dhdH, 
				out float mu, out float muS, out float nu)
{
    const float x = pixelX;
    const float y = pixelY;
    if (y < float(MR_BrunetonAtmConstants.const_ResMU) / 2.0) 
    {
        float d = 1.0 - y / (float(MR_BrunetonAtmConstants.const_ResMU) / 2.0 - 1.0);
        d = min(max(dhdH.z, d * dhdH.w), dhdH.w * 0.999);
        mu = (MR_BrunetonAtmConstants.const_Rg * MR_BrunetonAtmConstants.const_Rg - r * r - d * d) / (2.0 * r * d);
        mu = min(mu, -sqrt(1.0 - (MR_BrunetonAtmConstants.const_Rg / r) * (MR_BrunetonAtmConstants.const_Rg / r)) - 0.001);
    } 
    else 
    {
        float d = (y - float(MR_BrunetonAtmConstants.const_ResMU) / 2.0) / (float(MR_BrunetonAtmConstants.const_ResMU) / 2.0 - 1.0);
        d = min(max(dhdH.x, d * dhdH.y), dhdH.y * 0.999);
        mu = (MR_BrunetonAtmConstants.const_Rt * MR_BrunetonAtmConstants.const_Rt - r * r - d * d) / (2.0 * r * d);
    }
    muS = ModGLSL(x, float(MR_BrunetonAtmConstants.const_ResMUS)) / (float(MR_BrunetonAtmConstants.const_ResMUS) - 1.0);
    // paper formula
    //muS = -(0.6 + log(1.0 - muS * (1.0 -  exp(-3.6)))) / 3.0;
    // better formula
    muS = tan((2.0 * muS - 1.0 + 0.26) * 1.1) / tan(1.26 * 1.1);
    nu = -1.0 + floor(x / float(MR_BrunetonAtmConstants.const_ResMUS)) / (float(MR_BrunetonAtmConstants.const_ResNU) - 1.0) * 2.0;
}


float2 TransmittanceUV(float r, float mu)
{
	float uR = sqrt((r - MR_BrunetonAtmConstants.const_Rg) / (MR_BrunetonAtmConstants.const_Rt - MR_BrunetonAtmConstants.const_Rg));
	float uMu = atan((mu + 0.15) / (1.0 + 0.15) * tan(1.5)) / 1.5;

	return float2(uMu, uR);
}

// Altitude
float TransmittanceR(float r)
{
	return MR_BrunetonAtmConstants.const_Rg + (r * r) * (MR_BrunetonAtmConstants.const_Rt - MR_BrunetonAtmConstants.const_Rg);
}

// This is the angle of view and zenith
float TransmittanceMu(float muS) 
{
	return -0.15f + tan(1.5f * muS) / tan(1.5f) * (1.0f + 0.15f);
}

float IrradianceR(float pixelY, float targetHeight)
{
	return MR_BrunetonAtmConstants.const_Rg + (pixelY) / (targetHeight - 1.0) * (MR_BrunetonAtmConstants.const_Rt - MR_BrunetonAtmConstants.const_Rg);
}

float IrradianceMuS(float pixelX, float targetWidth)
{
	return -0.2 + (pixelX) / (targetWidth - 1.0) * (1.0 + 0.2);
}

float2 IrradianceUV(float r, float muS)
{
    float uR = (r - MR_BrunetonAtmConstants.const_Rg) / (MR_BrunetonAtmConstants.const_Rt - MR_BrunetonAtmConstants.const_Rg);
    float uMuS = (muS + 0.2) / (1.0 + 0.2);
    return float2(uMuS, uR);
}

void LayerData(int layer, out float r, out float4 dhdH)
{
    r = layer / (MR_BrunetonAtmConstants.const_ResR - 1.0);
    r = r * r;
    r = sqrt(MR_BrunetonAtmConstants.const_Rg * MR_BrunetonAtmConstants.const_Rg + r * (MR_BrunetonAtmConstants.const_Rt * MR_BrunetonAtmConstants.const_Rt - MR_BrunetonAtmConstants.const_Rg * MR_BrunetonAtmConstants.const_Rg)) + (layer == 0 ? 0.01 : (layer == MR_BrunetonAtmConstants.const_ResR - 1 ? -0.001 : 0.0));
    float dmin = MR_BrunetonAtmConstants.const_Rt - r;
    float dmax = sqrt(r * r - MR_BrunetonAtmConstants.const_Rg * MR_BrunetonAtmConstants.const_Rg) + sqrt(MR_BrunetonAtmConstants.const_Rt * MR_BrunetonAtmConstants.const_Rt - MR_BrunetonAtmConstants.const_Rg * MR_BrunetonAtmConstants.const_Rg);
    float dminp = r - MR_BrunetonAtmConstants.const_Rg;
    float dmaxp = sqrt(r * r - MR_BrunetonAtmConstants.const_Rg * MR_BrunetonAtmConstants.const_Rg);
    dhdH = float4(dmin, dmax, dminp, dmaxp);
}

float PlanetAtm_RayIntersect(float r, float mu)
{
    float dout = -r * mu + sqrt(r * r * (mu * mu - 1.0f) + MR_BrunetonAtmConstants.const_RL * MR_BrunetonAtmConstants.const_RL);
    float delta2 = r * r * (mu * mu - 1.0f) + MR_BrunetonAtmConstants.const_Rg * MR_BrunetonAtmConstants.const_Rg;
    if (delta2 >= 0.0f)
    {
        float din = -r * mu - sqrt(delta2);
        if (din >= 0.0f)
        {
            dout = min(dout, din);
        }
    }
    return dout;
}

// optical depth for ray (r,mu) of length d, using analytic formula
// (mu=cos(view zenith angle)), intersections with ground ignored
// H=height scale of exponential density function
float OpticalDepth(float H, float r, float mu, float d)
{
    float a = sqrt((0.5/H)*r);
    float2 a01 = a*float2(mu, mu + d / r);
    float2 a01s = sign(a01);
    float2 a01sq = a01*a01;
    float x = a01s.y > a01s.x ? exp(a01sq.x) : 0.0;
    float2 y = a01s / (2.3193*abs(a01) + sqrt(1.52*a01sq + 4.0)) * float2(1.0, exp(-d/H*(d/(2.0*r)+mu)));
    return sqrt((6.2831*H)*r) * exp((MR_BrunetonAtmConstants.const_Rg-r)/H) * (x + dot(y, float2(1.0, -1.0)));
}


// transmittance(=transparency) of atmosphere for ray (r,mu) of length d
// (mu=cos(view zenith angle)), intersections with ground ignored
// uses analytic formula instead of transmittance texture
float3 AnalyticTransmittance(float r, float mu, float d) 
{
    return exp(- MR_BrunetonAtmConstants.const_BetaR * OpticalDepth(MR_BrunetonAtmConstants.const_HR, r, mu, d) - (MR_BrunetonAtmConstants.const_BetaMSca / 0.9) * OpticalDepth(MR_BrunetonAtmConstants.const_HM, r, mu, d));
}

// transmittance(=transparency) of atmosphere for infinite ray (r,mu)
// (mu=cos(view zenith angle)), intersections with ground ignored
float3 Transmittance(float r, float mu)
{
	float2 uv = TransmittanceUV(r, mu);
    return MR_SampleLod0(TransmittanceTexture, uv).rgb;
}

// transmittance(=transparency) of atmosphere between x and x0
// assume segment x,x0 not intersecting ground
// d = distance between x and x0, mu=cos(zenith angle of [x,x0) ray at x)
float3 TransmittanceXX0(float r, float mu, float d)
{
    float3 result;
    float r1 = sqrt(r * r + d * d + 2.0 * r * mu * d);
    float mu1 = (r * mu + d) / r1;
    if (mu > 0.0) 
    {
        result = min(Transmittance(r, mu) / Transmittance(r1, mu1), 1.0);
    } 
    else 
    {
        result = min(Transmittance(r1, -mu1) / Transmittance(r, -mu), 1.0);
    }
    return result;
}

float3 TransmittanceWithShadow(float r, float mu)
{
    return mu < -sqrt(1.0 - (MR_BrunetonAtmConstants.const_Rg / r) * (MR_BrunetonAtmConstants.const_Rg / r)) ? 0.0 : Transmittance(r, mu);
}

float3 Irradiance(MR_Sampler2D table, float r, float muS)
{
    float2 uv = IrradianceUV(r, muS);
    return MR_SampleLod0(table, uv).rgb;
}

// Rayleigh phase function
float PhaseFunctionR(float mu)
{
    return (3.0 / (16.0 * Bruneton_Pi)) * (1.0 + mu * mu);
}

// Mie phase function
// Mie phase function
float PhaseFunctionM(float mu, float mieG)
{
    return 1.5 * 1.0 / (4.0 * Bruneton_Pi) * (1.0 - mieG * mieG) * pow(1.0 + (mieG * mieG) - 2.0 * mieG * mu, -3.0/2.0) * (1.0 + mu * mu) / (2.0 + mieG * mieG);
}

float PhaseFunctionM(float mu)
{
    return PhaseFunctionM(mu, MR_BrunetonAtmConstants.const_MieG*MR_BrunetonAtmConstants.const_MieG);
}

float4 Texture4D(MR_Sampler3D table, float r, float mu, float muS, float nu)
{
    float H = sqrt(MR_BrunetonAtmConstants.const_Rt * MR_BrunetonAtmConstants.const_Rt - MR_BrunetonAtmConstants.const_Rg * MR_BrunetonAtmConstants.const_Rg);
    float rho = sqrt(r * r - MR_BrunetonAtmConstants.const_Rg * MR_BrunetonAtmConstants.const_Rg);

    float rmu = r * mu;
    float delta = rmu * rmu - r * r + MR_BrunetonAtmConstants.const_Rg * MR_BrunetonAtmConstants.const_Rg;
    float4 cst = rmu < 0.0 && delta > 0.0 ? float4(1.0, 0.0, 0.0, 0.5 - 0.5 / float(MR_BrunetonAtmConstants.const_ResMU)) : float4(-1.0, H * H, H, 0.5 + 0.5 / float(MR_BrunetonAtmConstants.const_ResMU));
	float uR = 0.5 / float(MR_BrunetonAtmConstants.const_ResR) + rho / H * (1.0 - 1.0 / float(MR_BrunetonAtmConstants.const_ResR));
    float uMu = cst.w + (rmu * cst.x + sqrt(delta + cst.y)) / (rho + cst.z) * (0.5 - 1.0 / float(MR_BrunetonAtmConstants.const_ResMU));
    // paper formula
    //float uMuS = 0.5 / float(RES_MU_S) + max((1.0 - exp(-3.0 * muS - 0.6)) / (1.0 - exp(-3.6)), 0.0) * (1.0 - 1.0 / float(RES_MU_S));
    // better formula
    float uMuS = 0.5 / float(MR_BrunetonAtmConstants.const_ResMUS) + (atan(max(muS, -0.1975) * tan(1.26 * 1.1)) / 1.1 + (1.0 - 0.26)) * 0.5 * (1.0 - 1.0 / float(MR_BrunetonAtmConstants.const_ResMUS));

    float lerpFactor = (nu + 1.0) / 2.0 * (float(MR_BrunetonAtmConstants.const_ResNU) - 1.0);
    float uNu = floor(lerpFactor);
    lerpFactor = lerpFactor - uNu;

    return MR_SampleLod0(table, float3((uNu + uMuS) / float(MR_BrunetonAtmConstants.const_ResNU), uMu, uR)) * (1.0 - lerpFactor) +
           MR_SampleLod0(table, float3((uNu + uMuS + 1.0) / float(MR_BrunetonAtmConstants.const_ResNU), uMu, uR)) * lerpFactor;
}

// approximated single Mie scattering (cf. approximate Cm in paragraph "Angular precision")
float3 Mie(float4 rayMie) 
{ // rayMie.rgb=C*, rayMie.w=Cm,r
	return rayMie.rgb * rayMie.w / max(rayMie.r, 1e-4) * (MR_BrunetonAtmConstants.const_BetaR.r / MR_BrunetonAtmConstants.const_BetaR);
}