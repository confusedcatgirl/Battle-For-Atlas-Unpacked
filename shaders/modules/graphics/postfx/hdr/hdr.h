import "shaders/modules/graphics/common_aces.h"

MR_Sampler2D Color : MR_Texture0 { filter = point, wrap = clamp };
MR_Sampler2D Bloom : MR_Texture1 { filter = linear point, wrap = clamp };
MR_Sampler2D BloomDirt : MR_Texture2 { filter = linear point, wrap = clamp };
MR_Sampler2D LensFlare : MR_Texture3 { filter = linear point, wrap = clamp };
MR_Sampler2D LensDirt : MR_Texture4 { filter = linear point, wrap = clamp };
@ifdef GFX_HDR_LUT
MR_Sampler3D HDRLUT : MR_Texture5 { wrap = clamp  };
@endif
MR_Sampler2D DitherNoise { texture = "textures/random16x16.tga", anisotropy = 0 };
	
constants : MR_ConstantBuffer0
{
	float2 myBloomUVOffset;
	float2 myBloomUVScale;
	float2 myBloomUVMin;
	float2 myBloomUVMax;
	float2 myLensFlareUVOffset;
	float2 myLensFlareUVScale;
	float2 myLensFlareUVMin;
	float2 myLensFlareUVMax;
	float myBloomDirtDisabledScale;
	float myBloomDirtScale;
}

constants HDR : MR_ConstantBuffer1
{
	float Exposure;
	float White2;

	float ColorGamma;
	float IntensityGamma;
	float ColorAdjustmentScale;

	float ShoulderStrength;
	float LinearStrength;
	float LinearAngle;
	float ToeStrength;
	float ToeNumerator;
	float ToeDenominator;
	float LinearWhitePoint;

	float CustomReinhardScale;
	float CustomReinhardOffset;

	float VignetteStrength;
	float VignetteOffset;
	float VignetteScale;
	float VignetteExponent;
}

@ifdef GFX_HDR_PQ
constants : MR_ConstantBuffer3
{
	float LUTScale;
	float LUTOffset;
	float NitsForPaperWhite;
}
@else
@ifdef GFX_HDR_LUT
constants : MR_ConstantBuffer3
{
	float LUTScale;
	float LUTOffset;
	float NitsForPaperWhite;
}
@endif
@endif

float3 RGBToXYZ(float3 aRGB)
{
	const float3x3 RGBToXYZMat = float3x3(0.4125, 0.3576, 0.1805, 0.2126, 0.7152, 0.0722, 0.0193, 0.1192, 0.9505);
	return MR_Transform(RGBToXYZMat, aRGB);
}

float3 XYZToRGB(float3 aXYZ)
{
	const float3x3 XYZToRGBMat = float3x3(3.2410, -1.5374, -0.4986, -0.9692, 1.8760, 0.0416, 0.0556, -0.2040, 1.0570);
	return MR_Transform(XYZToRGBMat, aXYZ);
}

float Lab_f(float t)
{
	if (t > pow(6.0 / 29.0, 3.0))
		return pow(t, 1.0 / 3.0);
	return pow(29.0 / 6.0, 2) * t / 3 + 4.0 / 29.0;
}

float Lab_invf(float t)
{
	if (t > 6.0 / 29.0)
		return pow(t, 3.0);
	return 3.0 * pow(6.0 / 29.0, 2.0) * (t - 4.0 / 29.0);
}

float3 RGBToLab(float3 aRGB)
{
	float3 XYZ = RGBToXYZ(aRGB);

	float L = 166.0 * Lab_f(XYZ.y) - 16.0;
	float a = 500.0 * (Lab_f(XYZ.x) - Lab_f(XYZ.y));
	float b = 200.0 * (Lab_f(XYZ.y) - Lab_f(XYZ.z));

	return float3(L,a,b);
}

float3 LabToRGB(float3 aLab)
{
	float L = aLab.x;
	float a = aLab.y;
	float b = aLab.z;

	float t = (L + 16.0) / 116.0;

	float Y = Lab_invf(t);
	float X = Lab_invf(t + a / 500.0);
	float Z = Lab_invf(t - b / 200.0);

	return XYZToRGB(float3(X,Y,Z));
}

float3 RGBToYxy(float3 aRGB)
{
	float3 XYZ = RGBToXYZ(aRGB);
	float3 Yxy;
	Yxy.x = XYZ.g;
	float temp = dot(float3(1.0f, 1.0f, 1.0f), XYZ);
	Yxy.yz = XYZ.xy / temp;
	return Yxy;
}

float3 YxyToRGB(float3 aYxy)
{
	float3 XYZ;
	float temp = aYxy.x / aYxy.z;
	XYZ.x = aYxy.y * temp;
	XYZ.y = aYxy.x;
	XYZ.z = (1.0f - aYxy.y - aYxy.z) * temp;
	return XYZToRGB(XYZ);
}

float3 FilmicTonemappingCustomizable(float3 aIn)
{
	float A = HDR.ShoulderStrength;
	float B = HDR.LinearStrength;
	float C = HDR.LinearAngle;
	float D = HDR.ToeStrength;
	float E = HDR.ToeNumerator;
	float F = HDR.ToeDenominator;
	return ((aIn * (A*aIn + C*B) + D*E)/(aIn * (A*aIn + B) + D*F)) - E/F;
}

float3 FilmicTonemapping(float3 aIn)
{
	float3 x = max(vec3(0), aIn - 0.004f);
	float3 gammaColor = (x*(6.2f*x + 0.5f)) / (x*(6.2f*x + 1.7f) + 0.06f);
	return pow(gammaColor, vec3(2.2));
}

float3 CustomReinhardTonemapping(float3 aIn)
{
	float3 result = aIn * HDR.CustomReinhardScale;
	return result / (result + vec3(HDR.CustomReinhardOffset));
}

float3 CustomReinhardYxy(float3 aIn)
{
	float Y = aIn.x;
	float2 xy = aIn.yz;

	float2 white_xy = RGBToYxy(vec3(1)).yz;
	float colrange = 0.06;
	float2 color = xy - white_xy;

	xy = color + white_xy;

	return float3(Y, xy);
}

float3 Calibrate(float3 rgb)
{
	rgb *= HDR.Exposure;
	return rgb;
}

@ifdef GFX_HDR_PQ
float3 NormalizeHDRSceneValue(float3 hdrSceneValue)
{
	const float MaxNitsFor2084 = 10000.0f;
    float3 normalizedLinearValue = hdrSceneValue * NitsForPaperWhite / MaxNitsFor2084;
    return normalizedLinearValue;
}

// Apply the ST.2084 curve to linear values and outputs non-linear values, normalized [0..1]
float3 NormalizedLinearToST2084(float3 normalizedLinearValue)
{
    float3 ST2084 = pow((0.8359375f + 18.8515625f * pow(abs(normalizedLinearValue), 0.1593017578f)) / (1.0f + 18.6875f * pow(abs(normalizedLinearValue), 0.1593017578f)), 78.84375f);
    return ST2084;
}

float3 LinearToPQ(float3 anHDRColor)
{
	const float3 normalizedHDRColor = NormalizeHDRSceneValue(anHDRColor);
	const float3 st2048Color = NormalizedLinearToST2084(normalizedHDRColor);

	return st2048Color;
}

@endif

@ifdef GFX_HDR_LUT
float3 HDRLut(float3 anHDRColor)
{
	return saturate(MR_SampleLod0(HDRLUT, anHDRColor * LUTScale + LUTOffset).rgb);
}
@endif

float3 ToneMap(float3 rgb)
{
	// Color adjustment
	float3 col = normalize(rgb);
	float3 intensity = rgb / col;

	col = pow(col, HDR.ColorGamma);
	intensity = pow(intensity, HDR.IntensityGamma);
	float3 rgbColorAdjusted = col * intensity * HDR.ColorAdjustmentScale;

	@ifdef GFX_HDR_YXY
		float3 Yxy = RGBToYxy(rgbColorAdjusted);
		float3 tonemapped Yxy;
	@else
		float3 tonemapped = rgbColorAdjusted;
	@endif

	// Tone mapping
	@ifdef GFX_HDR_ACES
		tonemapped = ACESFitted(rgb);
	@else
	@ifdef GFX_HDR_PQ
		tonemapped = LinearToPQ(rgb);
	@else
	@ifdef GFX_HDR_REINHARD
		tonemapped = tonemapped / (1.0f + tonemapped);
	@else				
	@ifdef GFX_HDR_REINHARD_MODIFIED
		tonemapped = (tonemapped * (1.0f + (tonemapped / HDR.White2))) / (1.0f + tonemapped);
	@else
	@ifdef GFX_HDR_FILMIC
		@ifdef GFX_HDR_YXY
			tonemapped = FilmicTonemapping(vec3(tonemapped)).x;
		@else
			tonemapped = FilmicTonemapping(tonemapped);
		@endif
	@else				
	@ifdef GFX_HDR_FILMIC_CUSTOMIZABLE
		@ifdef GFX_HDR_YXY
			tonemapped = FilmicTonemappingCustomizable(vec3(tonemapped)).x / FilmicTonemappingCustomizable(vec3(HDR.LinearWhitePoint)).x;
		@else
			tonemapped = FilmicTonemappingCustomizable(tonemapped) / FilmicTonemappingCustomizable(vec3(HDR.LinearWhitePoint)).x;
		@endif
	@else
	@ifdef GFX_HDR_CUSTOM_REINHARD
		@ifdef GFX_HDR_YXY
			Yxy = CustomReinhardYxy(Yxy);
		@else
			tonemapped = CustomReinhardTonemapping(tonemapped);
		@endif
	@else
		// nothing - linear
	@endif
	@endif
	@endif
	@endif
	@endif
	@endif
	@endif

	@ifdef GFX_HDR_YXY
		float3 color = YxyToRGB(Yxy);
	@else
		float3 color = tonemapped;
	@endif

	return color;
}

float3 CalculateFinalColor(float2 uv0, float2 uv0Min, float2 uv0Max, float2 uv1, float2 fc)
{
	float3 color = MR_SampleLod0(Color, uv0).xyz;
	
	@ifdef GFX_HDR_COLOR_CAPPING_PC
	//PC specific optimization where on some GPU the color was reaching infinity and hence causing visual artifacts
	const float colorCap=50000.0f;
	color=min(color,float3(colorCap,colorCap,colorCap));
	@endif

	@ifndef GFX_HDR_DISABLE		
		@ifdef GFX_HDR_BLOOM
			float2 bloomUV = clamp(myBloomUVOffset + uv1 * myBloomUVScale, myBloomUVMin, myBloomUVMax);
			float3 bloom = MR_SampleLod0(Bloom, bloomUV).rgb;

			float bloomAlpha = myBloomDirtDisabledScale;
			@ifdef GFX_HDR_BLOOM_DIRT
				bloomAlpha += MR_SampleLod0(BloomDirt, uv1).r * myBloomDirtScale;
			@endif

			color = lerp(color, bloom, saturate(bloomAlpha));
		@endif

		@ifdef GFX_HDR_LENS_FLARE
			float2 lensFlareUV = clamp(myLensFlareUVOffset + uv1 * myLensFlareUVScale, myLensFlareUVMin, myLensFlareUVMax);
			float3 lensFlare = MR_SampleLod0(LensFlare, lensFlareUV).rgb;
			float3 lensDirt = MR_SampleLod0(LensDirt, uv1).rgb;
			color += lensFlare * lensDirt;
		@endif
		
		color = Calibrate(color);

		@ifdef GFX_HDR_TONEMAP
		color = ToneMap(color);
		@endif

		@ifdef GFX_HDR_LUT
		color = HDRLut(color);
		@endif

		if (HDR.VignetteStrength > 0)
		{
			float2 localUV = (uv0 - uv0Min) / (uv0Max - uv0Min);
			float2 clip2 = localUV * 2 - vec2(1);

			float len = length(clip2);
			float t = saturate(pow(max(HDR.VignetteOffset - len, 0) * HDR.VignetteScale, HDR.VignetteExponent));
			color *= 1 - HDR.VignetteStrength*(1-t);
		}
	@else
		color = pow(color, 2.2);
	@endif

	return color;
}