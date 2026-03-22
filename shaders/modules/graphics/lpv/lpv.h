import "shaders/modules/graphics/environment.h"
import "shaders/MRender/MR_ShaderDebug.h"

// @define GFX_LPV_POINT_SAMPLING

MR_Sampler2DArray SHTexture1 : MR_GIDirections1
{
@ifdef GFX_LPV_POINT_SAMPLING
	filter = point,
@else
	filter = linear point,
@endif
	wrap = clamp,
};

MR_Sampler2DArray GeometryTexture : MR_GeometryTexture
{
@ifdef GFX_LPV_POINT_SAMPLING
	filter = point,
@else
	filter = linear point,
@endif
	wrap = clamp,
};

const int Gfx_NumLPVCascades = 4;

constants Gfx_LPV : MR_LPVConstants
{
	float4 CoordScale[4];
	float4 CoordBias[4];
	float4 Min[4];
	float4 Max[4];
	float SHFactor;
	float Resolution;
	float SampleOffset;
	float FirstCascadeCellSize;
	int NumCascades;
	int ForcedCascade;
}

bool IsInsideAABB(float3 aPos, float3 aMin, float3 aMax)
{
	return aPos.x >= aMin.x && aPos.y >= aMin.y && aPos.z >= aMin.z
		&& aPos.x <= aMax.x && aPos.y <= aMax.y && aPos.z <= aMax.z;
}

int GetLPVCascadeIndex(float3 aPos)
{
	if (Gfx_LPV.ForcedCascade != -1)
		return Gfx_LPV.ForcedCascade;

	int index;
	if (IsInsideAABB(aPos, Gfx_LPV.Min[1].xyz, Gfx_LPV.Max[1].xyz))
	{
		if (IsInsideAABB(aPos, Gfx_LPV.Min[0].xyz, Gfx_LPV.Max[0].xyz))
			index = 0;
		else
			index = 1;
	}
	else
	{
		if (IsInsideAABB(aPos, Gfx_LPV.Min[2].xyz, Gfx_LPV.Max[2].xyz))
			index = 2;
		else
			index = 3;
	}

	return min(index, Gfx_LPV.NumCascades-1);
}

int GetLPVCascadeIndexForRadius(float aRadius)
{
	if (Gfx_LPV.ForcedCascade != -1)
		return Gfx_LPV.ForcedCascade;

	float findex = log2(aRadius / Gfx_LPV.FirstCascadeCellSize);
	return clamp(int(findex + 0.5), 0, 3);
}

void GetLPVFactors(float3 Gfx_WorldSpacePos, out float4 aRedOut, out float4 aGreenOut,
	out float4 aBlueOut, out float4 aGeometryOut, float3 aNormal, int aMinCascade)
{
	int cascadeIndex = clamp(GetLPVCascadeIndex(Gfx_WorldSpacePos), aMinCascade, 3);

	float resolution = Gfx_LPV.Resolution;
	float halfTexel = 0.5 / resolution;

	float3 pos = Gfx_WorldSpacePos + aNormal * Gfx_LPV.SampleOffset / resolution;

	float3 tc3 = Gfx_LPV.CoordScale[cascadeIndex].xyz * Gfx_WorldSpacePos + Gfx_LPV.CoordBias[cascadeIndex].xyz;
	tc3 += aNormal * Gfx_LPV.SampleOffset;

	float3 tc3s = clamp(tc3, vec3(halfTexel), vec3(1 - halfTexel));

	aRedOut = vec4(0);
	aGreenOut = vec4(0);
	aBlueOut = vec4(0);
	aGeometryOut = vec4(0);

	float dist = dot(abs(tc3 - tc3s), vec3(1));
	if (!dist)
	{
		float z = (tc3s.z - halfTexel) * resolution;
		float iz = floor(z);
		float iz1 = min(iz + 1, resolution - 1);

@ifdef GFX_LPV_POINT_SAMPLING
		iz = floor(z + 0.5);
		iz1 = iz;
@endif

		float tz = z - iz;

		float2 tc0;
		tc0.x = tc3.x;
		tc0.y = (tc3s.y + iz) / resolution;

		float2 tc1;
		tc1.x = tc3.x;
		tc1.y = (tc3s.y + iz1) / resolution;

		float4 factor = float4(-1,-1,-1,1) * Gfx_LPV.SHFactor;

		int index = cascadeIndex * 3;
		aRedOut = factor * lerp(MR_SampleLod0(SHTexture1, float3(tc0, index + 0.0f)), MR_SampleLod0(SHTexture1, float3(tc1, index + 0)), tz);
		aGreenOut = factor * lerp(MR_SampleLod0(SHTexture1, float3(tc0, index + 1.0f)), MR_SampleLod0(SHTexture1, float3(tc1, index + 1)), tz);
		aBlueOut = factor * lerp(MR_SampleLod0(SHTexture1, float3(tc0, index + 2.0f)), MR_SampleLod0(SHTexture1, float3(tc1, index + 2)), tz);
		aGeometryOut = factor * lerp(MR_SampleLod0(GeometryTexture, float3(tc0, cascadeIndex)), MR_SampleLod0(SHTexture1, float3(tc1, cascadeIndex)), tz);
	}
// 	else if (dist < 0.01)
// 		aGreenOut = float4(0,0,0,1);
}

float4 GetLPVFactorsForChannel(float3 Gfx_WorldSpacePos, float3 aNormal, int aMinCascade, int aChannel)
{
	float4 r, g, b, geom;
	GetLPVFactors(Gfx_WorldSpacePos, r, g, b, geom, aNormal, aMinCascade);

	if (aChannel == 0)
		return r;
	if (aChannel == 1)
		return g;
	if (aChannel == 2)
		return b;
	return vec4(0);
}

float4 SampleLPVSHFactorsForChannel(float3 aWorldPos, float3 aNormal, int aChannel)
{
	float delta = 1;

	float4 r_p = GetLPVFactorsForChannel(aWorldPos + aNormal * delta * 0.5, aNormal, 0, aChannel);
	float4 r_n = GetLPVFactorsForChannel(aWorldPos - aNormal * delta * 0.5, aNormal, 0, aChannel);

// 	float4 r_p = GetLPVFactorsForChannel(aWorldPos + aNormal * delta, aNormal, 0, aChannel);
// 	float4 r_n = GetLPVFactorsForChannel(aWorldPos, aNormal, 0, aChannel);

	float4 r_d = (r_p - r_n) / delta;
	float4 r_t = (r_p + r_n) * 0.5;
// 	float4 r_t = r_n;

	float length_p = length(r_p.xyz);
	float length_n = length(r_n.xyz);

	float incomingDot = dot(r_t.xyz, aNormal);
	float deltaDot = r_d.w;
	float lengthDelta = length_p - length_n;
	float avgDelta = r_p.w - r_n.w;

	float4 r = r_t;

	if (MR_ShaderDebug.bool_3)
	{
		float factor = saturate(MR_ShaderDebug.float_3 * avgDelta);
		r *= factor;
	}

	return r;
}

void SampleLPVSHFactors(inout float4 aR, inout float4 aG, inout float4 aB, float3 aWorldPos, float3 aNormal)
{
	aR += SampleLPVSHFactorsForChannel(aWorldPos, aNormal, 0);
	aG += SampleLPVSHFactorsForChannel(aWorldPos, aNormal, 1);
	aB += SampleLPVSHFactorsForChannel(aWorldPos, aNormal, 2);

// 	float4 r_t, g_t, b_t, geom_t;
// 	GetLPVFactors(aWorldPos, r_t, g_t, b_t, geom_t, aNormal, 0);
// 	aR += r_t;
// 	aG += g_t;
// 	aB += b_t;
}

float3 GetLPVLight(float3 aWorldPos, float3 aNormal)
{
	float4 r_t, g_t, b_t, geom_t;
	GetLPVFactors(aWorldPos, r_t, g_t, b_t, geom_t, aNormal, 0);

	float4 v = float4(aNormal, 1);

	float3 col;
	col.x = max(0.0, dot(r_t, v));
	col.y = max(0.0, dot(g_t, v));
	col.z = max(0.0, dot(b_t, v));
	return col;
}

float3 GetLPVLightAndGeom(float3 aWorldPos, float3 aNormal, int aMinCascade, out float aGeomOut)
{
	float4 r_t, g_t, b_t, geom_t;
	GetLPVFactors(aWorldPos, r_t, g_t, b_t, geom_t, aNormal, aMinCascade);

	float4 v = float4(aNormal, 1);

	float3 col;
	col.x = max(0.0, dot(r_t, v));
	col.y = max(0.0, dot(g_t, v));
	col.z = max(0.0, dot(b_t, v));
	aGeomOut = max(0.0, dot(geom_t, v));
	return col;
}

void GetLPVReflectedLight(inout float3 aReflectedLight, float3 aPosition, float3 aDirection, float aRoughness)
{
	if (aRoughness > 0.99)
		return;

	int maxsteps = MR_ShaderDebug.int_0;

	int index = GetLPVCascadeIndex(aPosition);
	float cascadeCellSize = pow(2, float(index)) * Gfx_LPV.FirstCascadeCellSize;

	float stepSize = cascadeCellSize;
	float currentAlpha = aRoughness;

	float maxRadius = MR_ShaderDebug.float_2;

	float3 accumulatedLight = vec3(0);

	float dist = 0;
	float maxAlpha = 0;
	float3 pos = aPosition;
	for (int i=0; i<maxsteps; ++i)
	{
		pos += stepSize * aDirection;
		dist += stepSize;

		float radius = aRoughness * dist * MR_ShaderDebug.float_3;

		index = GetLPVCascadeIndexForRadius(radius);
		cascadeCellSize = pow(2, float(index)) * Gfx_LPV.FirstCascadeCellSize;
		stepSize = max(stepSize, cascadeCellSize);

		float weight = saturate(1.0 - radius / maxRadius);
		float contribution = weight * (1.0 - currentAlpha);

		if (contribution < 0.01)
			break;

		float alpha = 0;
		float3 light = GetLPVLightAndGeom(pos, aDirection, index, alpha);
		alpha = min(alpha, maxAlpha);

		accumulatedLight += light * contribution;
		currentAlpha = lerp(currentAlpha, 1.0, alpha);
// 		maxAlpha = 1;
	}

	if (MR_ShaderDebug.bool_2)
		aReflectedLight = accumulatedLight + aReflectedLight * (1.0 - currentAlpha);
	else
		aReflectedLight += accumulatedLight;
}
