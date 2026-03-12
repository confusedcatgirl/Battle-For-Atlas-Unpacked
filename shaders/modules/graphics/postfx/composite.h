import "shaders/modules/graphics/environment.h"
import "shaders/modules/graphics/postfx/chromaticaberration.h"
import "shaders/modules/graphics/postfx/sharpen.h"

MR_Sampler2D BackBuffer : MR_Texture0 { filter = point, wrap = clamp };
MR_Sampler2D BackBufferLinear : MR_Texture0, 1 { filter = linear point, wrap = clamp, anisotropy = 0 };
MR_Sampler3D LUTTexture : MR_Texture1 { wrap = clamp };
MR_Sampler2D BlurMasks : MR_Texture2 { wrap = clamp };
MR_Sampler2D BlurredBackBuffer : MR_Texture3 { wrap = clamp };
MR_Sampler2D Image : MR_Texture4 { wrap = clamp };
MR_Sampler2D Velocity : MR_Texture5 { wrap = clamp };
MR_Sampler2D BlurMaskUI : MR_Texture6 { wrap = clamp };

constants : MR_ConstantBuffer0
{
	float2 ProtoStyleBlurScale;
	float2 SourceSize;
	float2 ScalePixelCenter;
	float2 ChromaticUvScale;

	float3 UIColorAdd;
	float UIBlurIntensityLimit;
	float3 UIColorMult;

	float BlurStrength;

	float3 Aberration;

	float LUTScale;
	float LUTOffset;

	float MotionBlurSamples;
	float MotionBlurScale;

	float ImageStrength;

	float Fade;
	float Saturation;
	float Contrast;

	float SharpenStrength;

	float2 PixelToBlurMaskUVScale;
	float2 PixelToBlurMaskUVOffset;
}

float3 FinalComposite(float2 uv0, float2 uv0Min, float2 uv0Max, float2 uv1, float2 uv1Min, float2 uv1Max, float2 fc)
{
	@ifdef USE_SCALING
		float2 scaleduv = uv0;

		{
			// pull sample positions towards texel centers for upscaling, and away from them for downscaling
			float2 fpixel = scaleduv * SourceSize - ScalePixelCenter;
			fpixel = floor(fpixel) + smoothstep(vec2(0), vec2(1), frac(fpixel));
			scaleduv = (fpixel + ScalePixelCenter) / SourceSize;
		}

		@ifdef USE_CHROMATICABERRATION
			float3 color = ChromaticAberrateColorsRadial(Aberration, BackBufferLinear, scaleduv, ChromaticUvScale, uv0Min, uv0Max);
		@else
			float3 color = MR_SampleLod0(BackBufferLinear, clamp(scaleduv, uv0Min, uv0Max)).rgb;
		@endif

	@else
		@ifdef USE_CHROMATICABERRATION
			float3 color = ChromaticAberrateColorsRadial(Aberration, BackBufferLinear, uv0, ChromaticUvScale, uv0Min, uv0Max);
		@else
			@ifdef USE_SHARPEN
				float3 color = Sharpen(BackBufferLinear, uint2(fc.xy), SharpenStrength, uv0Min, uv0Max);
			@else
				float3 color = MR_SampleTexelLod0(BackBuffer, fc.xy).rgb;
			@endif
		@endif
	@endif

	@ifdef USE_BLUR
		float2 masks = MR_SampleLod0(BlurMasks, uv0).rg;
	@endif

	@ifdef USE_MOTION_BLUR
		float2 velocity = MR_SampleLod0(Velocity, uv0).xy;
		velocity *= float2(0.5, -0.5) * (uv0Max - uv0Min);

		velocity /= MotionBlurSamples;
		velocity *= MotionBlurScale;// * masks.g;

		float2 texCoord = uv0;
		float4 motionColor = vec4(0);
		for(float i = 0; i < MotionBlurSamples; ++i, texCoord += velocity)
		{
			float2 clamped = clamp(texCoord, uv0Min, uv0Max);
			if (clamped.x != texCoord.x || clamped.y != texCoord.y)
				break;

			motionColor += float4(MR_SampleLod0(BackBuffer, texCoord).rgb, 1.0);
		}
		color = motionColor.xyz / motionColor.w;
	@endif

	@ifdef USE_PROTO_STYLE_BLUR
		float w1 = (10.0/50.0);
		float w2 = (6.0/50.0);
		float w3 = (4.0/50.0);

		float2 wi = float2(ProtoStyleBlurScale.x, 0.0) * masks.g;
		float2 hi = float2(0.0, ProtoStyleBlurScale.y) * masks.g;

		float3 t0 = MR_SampleLod0(BackBuffer, clamp(uv0, uv0Min, uv0Max)).rgb * w1;
		float3 t1 = MR_SampleLod0(BackBuffer, clamp(uv0 + wi, uv0Min, uv0Max)).rgb * w2;
		float3 t2 = MR_SampleLod0(BackBuffer, clamp(uv0 - wi, uv0Min, uv0Max)).rgb * w2;
		float3 t3 = MR_SampleLod0(BackBuffer, clamp(uv0 + hi, uv0Min, uv0Max)).rgb * w2;
		float3 t4 = MR_SampleLod0(BackBuffer, clamp(uv0 - hi, uv0Min, uv0Max)).rgb * w2;
		float3 t5 = MR_SampleLod0(BackBuffer, clamp(uv0 + wi + hi, uv0Min, uv0Max)).rgb * w3;
		float3 t6 = MR_SampleLod0(BackBuffer, clamp(uv0 + wi - hi, uv0Min, uv0Max)).rgb * w3;
		float3 t7 = MR_SampleLod0(BackBuffer, clamp(uv0 - wi + hi, uv0Min, uv0Max)).rgb * w3;
		float3 t8 = MR_SampleLod0(BackBuffer, clamp(uv0 - wi - hi, uv0Min, uv0Max)).rgb * w3;

		color = (t0 + t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8);
	@endif

	@ifdef USE_BLUR
	  float2 blurMaskUV = fc.xy * PixelToBlurMaskUVScale + PixelToBlurMaskUVOffset;
	  float uiblurAmount = MR_SampleLod0(BlurMaskUI, blurMaskUV).r;
	  float blurAmount = max(masks.r * BlurStrength, uiblurAmount);
		if(blurAmount > 0.0)
		{
			float3 blur = MR_SampleLod0(BlurredBackBuffer, clamp(uv1, uv1Min, uv1Max)).rgb;
			color = lerp(color, blur, blurAmount);

			float darkAmount = min(uiblurAmount*2, 1);

			float len = length(color);
			float diff = min(len, 1.732 * (1 - UIBlurIntensityLimit * darkAmount));

			color = color * (diff / max(len, 0.0001));
		}
	@endif

	@ifdef USE_LUT
		// Color Transform
		color = MR_SampleLod0(LUTTexture, color * LUTScale + LUTOffset).rgb;
	@endif

	@ifdef USE_IMAGE
		if (ImageStrength > 0.0)
		{
			float2 viewportUV = MR_PixelToClip(fc) * float2(0.5, -0.5) + float2(0.5, 0.5);
			float4 fullImg = MR_SampleLod0(Image, viewportUV);
			color = lerp(color, fullImg.rgb, fullImg.a * ImageStrength);
		}
	@endif

	@ifdef USE_POSTPROCESSING
		const float3 lumWeights = float3(0.2126, 0.7152, 0.072);
		color = lerp(vec3(dot(lumWeights, color)), color, Saturation);
		color = lerp(vec3(0), color, Fade);
		color = pow(color, vec3(Contrast));
	@endif

	return (color * UIColorMult) + UIColorAdd;
}

