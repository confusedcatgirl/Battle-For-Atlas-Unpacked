import "shaders/MRender/MR_ViewConstants.h"

import "shaders/modules/graphics/deferred_data.h"
import "shaders/modules/graphics/lighting_data.h"
import "shaders/modules/graphics/normal_encode.h"

import "shaders/MRender/MR_DepthBuffer.h"

compilerHints { important }

MR_Texture2D MRT0 : MR_Texture0;
MR_Texture2D MRT1 : MR_Texture1;
MR_Sampler2D MRT2 : MR_Texture2 { filter = linear point, wrap = clamp };
MR_Texture2D<float> Depth : MR_Texture3;
MR_Texture2D<float3> Normal : MR_Texture4 { mutable = true };

MR_Sampler2D<float3> Light_RGB : MR_Texture5 { filter = linear point, wrap = clamp };
MR_Sampler2D<float2> Light_RadiusAlpha : MR_Texture15 { filter = linear point, wrap = clamp };
MR_Sampler2D<float3> OldLight_RGB : MR_Texture6 { filter = linear point, wrap = clamp };
MR_Sampler2D<float2> OldLight_RadiusAlpha : MR_Texture14 { filter = linear point, wrap = clamp };

MR_Texture2D<float> DownscaledMaxRadius : MR_Texture7;
MR_Texture2D<float4> Hits : MR_Texture8;
MR_Texture2D<float3> BlendSource_RGB : MR_Texture9;
MR_Texture2D<float2> Distance_Alpha : MR_Texture11;

MR_Texture2D<float3> QuarterNormal : MR_QuarterNormalTexture;

MR_Texture2D<float2> MotionVectors : MR_Texture10;

constants SSR : MR_ConstantBuffer0
{
	float4 sssDermisColor;
	float2 viewportOffset;
	float2 quarterSizeSinglePixelUVOffset;
	int2 fullSourceMin;
	int2 fullSourceMax;
	int2 quarterSourceMin;
	int2 quarterSourceMax;
	float2 jitter;
	float2 quarterPixelToLastFrameUVScale;
	float2 quarterClipToLastFrameUVScale;
	float2 lastFrameClipToFramebufferUVScale;
	float2 lastFrameClipToFramebufferUVOffset;
	float range;
	float roughLimit;
	float roughNumSamples;
	float numSamples;
	float maxRays;
	float maxRoughness;
	float roughFirstStepSize;
	float firstStepSize;
	float stepFactor;
	float maxStepSize;
	float alphaRange;
	float maxRadiusAlphaCutoff;
	float blurNormalDampening;
	float thisFrameAlpha;
	float flattenGroundThreshold;
	float finalSampleMaxNormalScale;
	float finalSampleOffsetScale;
	float finalSampleMaxDepthDiff;
	float finalSampleMinAlphaFraction;
	float finalSampleMaxNormalDiff;
	float quarterPixelSizeScale;
	float ssr2BlurScale;
	float ssr2BlurOffset;
	float ssr2NormalOffsetScale;
	float sssScale;
	float sssAlpha;
	float sssSkinAlpha;
	uint frameIndex;
}

constants SSRViewport : MR_ConstantBuffer1
{
	int2 pixelMin;
	int2 pixelMax;
	float2 direction;
}

const int MaxRadiusScale = 4;

const float RadiusEncodingScale = 32.0;

float2 EncodeRadiusAlpha(float radius, float alpha, float sign_)
{
	float2 t = float2(radius / RadiusEncodingScale, alpha);
	return t;
}

void DecodeRadiusAlpha(out float radius, out float alpha, out float signOut, float2 aRadiusAlpha)
{
@ifdef SSS
	aRadiusAlpha = abs(aRadiusAlpha);
	signOut = -1;
@else
	signOut = 1;
@endif

	radius = aRadiusAlpha.x * RadiusEncodingScale;
	alpha = aRadiusAlpha.y;
}

float GetPackedNormalWeightFactor(float3 aPackedNormal1, float3 aPackedNormal2)
{
	float3 t = aPackedNormal1 - aPackedNormal2;
	float proximityFactor = SSR.blurNormalDampening + dot(t,t);
	return 1.0 / proximityFactor;
}
