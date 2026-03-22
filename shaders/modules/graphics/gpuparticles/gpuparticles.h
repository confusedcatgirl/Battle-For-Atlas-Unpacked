import "shaders/modules/graphics/environment.h"
import "shaders/modules/graphics/normal_encode.h"
import "shaders/modules/graphics/deferred_data.h"

blend {
	src = 1,
	dest = 0,
	writeMask = rgba
}

constants : MR_ConstantBuffer0
{
	float RainOnGroundAmount;
	float RainOnSnowAmount;
	float NormalMinY;
	float NormalBlend;
}

// samplers
MR_Sampler2D SnowD { texture = "textures/modules/graphics/tt_ss_snow.dds", filter = linear linear };
MR_Sampler2D SnowN { texture = "textures/modules/graphics/tt_ss_snow_n.dds", filter = linear linear };
MR_Sampler2D SparkleD { texture = "textures/modules/graphics/tt_snow_sparkles.dds", filter = point point };

float2  triplanarUV ( float3 pos, float3 normal)
{ 
	float3 absN = abs(normal);
	float maxN = max(max(absN.x, absN.y), absN.z);
	float3 stepN = step(maxN, absN);
	float2 UV = lerp(lerp(pos.xy, pos.zy, stepN.x), pos.zx, stepN.y);
	
	return UV;
}

bool ModifyGBuffer(inout Gfx_DeferredData data, float3 pos, float depth, float3 normal, float weatherMask, float3 snowDecalNormal, float snowNormalBlend, float snowMeltingFactor)
{
	// EARLY OUT
	if (normal.y <= NormalMinY) return false;

	// UV's
	float2 triUV = triplanarUV(pos, normal);
	
	float2 snowUV = triUV * 0.5;
	float2 patchUV = pos.xz * 0.125;
	float2 normalUV = pos.xz * 0.25;
	float2 sparkleUV = triUV * 0.25;

	// Mip level
	float lod = MR_GetLod(SnowD, snowUV);
	float normalLod = MR_GetLod(SnowN, normalUV);
	float patchLod = MR_GetLod(SnowD, patchUV);
	
	// snow textures
	float4 snowMap = MR_SampleLod(SnowD, snowUV, lod);
	float patches = MR_SampleLod(SnowD, patchUV, lod).b;
	float sparkle = MR_SampleLod(SparkleD, sparkleUV, patchLod).g;
	
	// prepare
	float snowAmount = saturate(Gfx_Environment.snowAmount) * weatherMask * (1.0 - 0.6 * snowMeltingFactor);
	float normalGradient = smoothstep(NormalMinY, 1.0, normal.y);
	
	// subtract stuff from snow mask
	float snowAlpha = saturate(snowAmount * (snowMap.y + 0.26) * 4.0 * normalGradient); // gradual noise to full white
	snowAlpha -= lerp(1-snowAmount, 0, data.ao) * snowMap.r; // mask AO less with more snow
	snowAlpha -= (1-snowAmount) * snowMap.r * patches * 4.0; // mask by snow amount
	snowAlpha -= (1-normalGradient) * snowMap.r * 3.0; // mask by normal
	snowAlpha = saturate(snowAlpha * 2.0); // clamp the subtractions

@ifdef SNOW_WRITE_TO_NORMAL	
@ifdef NORMAL_BLEND
	{
		// normal mapping
		float3 tangentWS = cross(normal, float3(1,0,0));
		float3 binormalWS = cross(normal, tangentWS);
		float3 snowNormal = MR_SampleLod(SnowN, normalUV, normalLod).xyz * 2 - 1;
		float3 snowNormalWS = normalize(tangentWS*snowNormal.x + binormalWS*snowNormal.y + normal*snowNormal.z);

		normal = normalize(lerp(normal, snowNormalWS, snowAlpha * normalGradient * snowAmount * NormalBlend));
	}
@endif

@ifdef SNOW_DECALS
	if(snowNormalBlend * snowAlpha > 0.1)
	{
		normal = lerp(normal, snowDecalNormal, snowNormalBlend * snowAlpha);

@ifndef NORMAL_BLEND
		data.worldNormal = normalize(normal);
@endif
	}
@endif

@ifdef NORMAL_BLEND
	data.worldNormal = normalize(normal);
@endif
@endif

	//snow material
	float3 snowColor = saturate(lerp(0.5, 0.8, snowMap.r * snowAlpha) + sparkle);
	float snowRoughness = lerp(0.9, 0.0, sparkle);
	float snowMetalness = 0.0;
	float snowMinReflection = lerp(0.06, 1.0, sparkle);
	float snowSSS = 0.075;

	//blend snow
	data.ao = data.ao;
	data.color = lerp(data.color, snowColor, snowAlpha);
	data.roughness = lerp(data.roughness, snowRoughness, saturate(snowAlpha*2));
	data.metalness = lerp(data.metalness, snowMetalness, snowAlpha);

	// transmission and sss can't be used at the same time - if transmission is used,
	// first fade it out over alpha [0-0.5], then fade in sss over [0.5-1]
	if (data.opacity < 1)
	{
		data.opacity = lerp(data.opacity, 1, snowAlpha);
		data.sss = lerp(data.sss, snowSSS, saturate(snowAlpha * 2 - 1));
	}
	else
	{
		data.opacity = 1;
		data.sss = lerp(data.sss, snowSSS, snowAlpha);
	}

	return true;
}
