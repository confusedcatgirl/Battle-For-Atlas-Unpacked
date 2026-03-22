@define SSR2

@include "shaders/modules/graphics/deferred/ssr_calculate.h"

import "shaders/nodes/Shader/Functions/Lighting/Reflected Light (Envmap).h"

MR_Sampler2D<float> ZMips : MR_Texture12 { filter = point, wrap = clamp };

MR_Texture2D<float3> Framebuffer : MR_Framebuffer0;
MR_Sampler2D<float3> LightMips : MR_Framebuffer1 { filter = linear, wrap = clamp, anisotropy = 0 };

float2 clipToUV(float2 c)
{
	return c * float2(0.5,-0.5) + 0.5;
}

float GetLightLod(float roughness, float3 clip1, float3 clip2)
{
	return log2(roughness * length(clip1 - clip2)) * SSR.ssr2BlurScale + SSR.ssr2BlurOffset;
}

float3 SampleLight(float2 clip, float lod)
{
@ifdef SSR2_ALLOW_FULL_RES_SAMPLES
	if (lod < 0.5)
		return MR_SampleTexelLod0(Framebuffer, MR_ClipToPixel(clip.xy));
	else
@endif
		return MR_SampleLod(LightMips, clipToUV(clip.xy), lod-1);
}

float SampleZ(float2 clip, int lod)
{
@ifdef SSR2_ZPYRAMID
@	ifdef SSR2_ALLOW_FULL_RES_SAMPLES
	if (lod < 1)
		return MR_SampleTexelLod0(DepthBufferZ, MR_ClipToPixel(clip.xy));
	else
@	endif
		return MR_SampleLod(ZMips, clipToUV(clip.xy), lod-1);
@else
	return MR_SampleTexelLod0(DepthBufferZ, MR_ClipToPixel(clip.xy));
@endif
}

float4 SSR2_GetReflectionSingleZ(
	float3 aStartPos,
	float2 aFFC,
	float aNoise,
	float aDepth,
	float3 aDirection,
	float3 aNormal,
	float aRoughness,
	bool aSimple
)
{
	float3 dummyStepPos = aStartPos + aDirection * 0.01;

	float4 startClip4 = MR_Transform(MR_ViewConstants.worldToClip, aStartPos);
	float4 dummyStepClip4 = MR_Transform(MR_ViewConstants.worldToClip, dummyStepPos);

	float3 startClip = startClip4.xyz / startClip4.w;
	float3 dummyStepClip = dummyStepClip4.xyz / dummyStepClip4.w;

	float3 start = startClip;
	float3 dummyStep = dummyStepClip;

	float stepSize = (2.0 * 2.0) / MR_ViewConstants.viewportSize.x;

	float3 diff = dummyStep - start;
	float3 rayDir = diff / max(0.000001, length(diff.xy));
	float3 stepV = rayDir * stepSize;

	float3 current = start + (0.5 + 0.5 * aNoise) * stepV;

	int samplesRemaining = 100;
	int lod = 0;
	while (samplesRemaining > 0)
	{
		--samplesRemaining;

		float3 next = current + stepV;

		if (abs(next.x) >= 1 || abs(next.y) >= 1 || next.z >= 1
@ifndef SSR_HIT_SKY
			|| next.z <= 0
@endif
		)
		{
			break;
		}

		float z = SampleZ(next.xy, lod);
		float zDiff = z - next.z;

		if (z > next.z)
		{
			if (aSimple)
				return vec4(1);
			else
			{
				//float t = abs(current.z - z) / abs(current.z - next.z);
				//current = lerp(current, next, saturate(t));

				if (lod <= 0 || samplesRemaining < 1)
				{
					float3 delta = float3(current.xy, z) - start;
					float slope = delta.z / max(0.000001, length(delta.xy));
					float alpha = abs(slope - rayDir.z) < 0.0001 ? 1 : 0;
					return float4(current, alpha);
				}

@ifdef SSR2_ZPYRAMID
				--lod;
				stepV /= 2;
@endif
			}
		}
		else
		{
@ifdef SSR2_ZPYRAMID
			if (lod < 3)
			{
				++lod;
				stepV *= 2;
			}
@endif

			current = next;
		}
	}

	return vec4(0);
}

void GetReflectionDirection(out float3 reflectOut, out float noiseOut, float3 normal, float3 toCamera, float roughness, float2 fc)
{
	uint2 noiseCoord = fc.xy;
	noiseCoord.x += SSR.frameIndex * 17;
	noiseCoord &= 255;

	float4 noise = Noise[noiseCoord];
	noiseOut = noise.w;

@ifdef SSR2_OFFSET_NORMALS

	float3 offset = vec3(0);
	offset = noise.xyz - vec3(0.5);
	offset /= pow(1000.0, 1.0 - roughness);
	offset *= saturate(roughness * 128.0);
	offset *= SSR.ssr2NormalOffsetScale;

	float3 offsetedNormal = normal + offset;
	float3 reflected = reflect(-toCamera, offsetedNormal);
	reflected = OW_Material_GetDominantSpecDir(reflected, offsetedNormal, roughness, roughness*roughness);

	if (dot(reflected, normal) > 0)
		reflectOut = normalize(reflected);
	else
@endif // SSR2_OFFSET_NORMALS

		reflectOut = normalize(reflect(-toCamera, normal));
}

float4 SSR2_GetReflectionSingle(
	float3 aStartPos,
	float2 aFFC,
	float aNoise,
	float aDepth,
	float3 aDirection,
	float3 aNormal,
	float aRoughness,
	bool aSimple
)
{
	uint numSamples = aRoughness > SSR.roughLimit ? SSR.roughNumSamples : SSR.numSamples;

@ifdef SSR2_Z
	return SSR2_GetReflectionSingleZ(aStartPos, aFFC, aNoise, aDepth, aDirection, aNormal, aRoughness, aSimple);
@else
	float blurRadius;
	return DSSR_GetReflectionSingle(aStartPos, aFFC, aNoise, aDepth, aDirection, aNormal, numSamples, aRoughness, blurRadius, aSimple);
@endif
}
