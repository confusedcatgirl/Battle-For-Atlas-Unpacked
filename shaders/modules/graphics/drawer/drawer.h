import "shaders/MRender/MR_ViewConstants.h"
import "shaders/MRender/MR_DepthBuffer.h"
import "shaders/modules/graphics/getstencil.h"
import "shaders/modules/graphics/drawer/drawerconstants.h"

pixel float4 sampleOutline(MR_Sampler2D tex, float2 tc)
{
	float2 xoffset = ddx(tc);
	float2 yoffset = ddy(tc);

	float2 tc0 = tc - xoffset;
	float2 tc1 = tc + xoffset;
	float2 tc2 = tc - yoffset;
	float2 tc3 = tc + yoffset;

	float2 tc4 = tc - xoffset - yoffset;
	float2 tc5 = tc - xoffset + yoffset;
	float2 tc6 = tc + xoffset + yoffset;
	float2 tc7 = tc + xoffset - yoffset;

	float4 sample0 = MR_SampleLod0(tex, tc0);
	float4 sample1 = MR_SampleLod0(tex, tc1);
	float4 sample2 = MR_SampleLod0(tex, tc2);
	float4 sample3 = MR_SampleLod0(tex, tc3);
	float4 max1 = max(max(sample0, sample1), max(sample2, sample3));

	float4 sample4 = MR_SampleLod0(tex, tc4);
	float4 sample5 = MR_SampleLod0(tex, tc5);
	float4 sample6 = MR_SampleLod0(tex, tc6);
	float4 sample7 = MR_SampleLod0(tex, tc7);
	float4 max2 = max(max(sample4, sample5), max(sample6, sample7));

	return max(max1, max2 * 3.1415 * 0.25);
}

pixel float4 sampleMask(MR_Sampler2D tex, float2 tc)
{
	float2 xoffset = ddx(tc) * 0.5;
	float2 yoffset = ddy(tc) * 0.5;

	float2 tc0 = tc - xoffset - yoffset;
	float2 tc1 = tc - xoffset + yoffset;
	float2 tc2 = tc + xoffset + yoffset;
	float2 tc3 = tc + xoffset - yoffset;

	float threshold = 0.5;
	float4 dist0 = threshold - MR_SampleLod0(tex, tc0);
	float4 dist1 = threshold - MR_SampleLod0(tex, tc1);
	float4 dist2 = threshold - MR_SampleLod0(tex, tc2);
	float4 dist3 = threshold - MR_SampleLod0(tex, tc3);

	float4 minDist = min(min(dist0, dist1), min(dist2, dist3));
	float4 maxDist = max(max(dist0, dist1), max(dist2, dist3));

	float4 range = maxDist - minDist;
	float4 average = dist0 + dist1 + dist2 + dist3;

	float4 aamask = saturate(0.5 - average / max(range * 2.0, 0.00001));
	return aamask;
}

pixel float2 sampleDistanceField(MR_Sampler2D tex, float2 tc)
{
	float2 xoffset = ddx(tc) * 0.5;
	float2 yoffset = ddy(tc) * 0.5;

	float2 tc0 = tc - xoffset - yoffset;
	float2 tc1 = tc - xoffset + yoffset;
	float2 tc2 = tc + xoffset + yoffset;
	float2 tc3 = tc + xoffset - yoffset;

	float threshold = 0.5;
	float dist0 = threshold - MR_SampleLod0(tex, tc0).r;
	float dist1 = threshold - MR_SampleLod0(tex, tc1).r;
	float dist2 = threshold - MR_SampleLod0(tex, tc2).r;
	float dist3 = threshold - MR_SampleLod0(tex, tc3).r;

	float minDist = min(min(dist0, dist1), min(dist2, dist3));
	float maxDist = max(max(dist0, dist1), max(dist2, dist3));

	float range = maxDist - minDist;
	float average = dist0 + dist1 + dist2 + dist3;

	float aamask = saturate(0.5 - average / max(range * 2.0, 0.00001));
	float outlinemask = saturate(0.5 - (average - range * 4.0) / max(range * 2.0, 0.00001));

	return float2(aamask, outlinemask);
}

// pixel float4 sampleHybridDistanceField(MR_Sampler2D tex, float2 tc)
// {
// 	float coverage = MR_SampleLod0(tex, tc).g;
// 
// 	float2 tcdx = ddx(tc);
// 	float2 tcdy = ddy(tc);
// 
// 	float2 xoffset = tcdx * 0.5;
// 	float2 yoffset = tcdy * 0.5;
// 
// 	float2 tc0 = tc - xoffset - yoffset;
// 	float2 tc1 = tc - xoffset + yoffset;
// 	float2 tc2 = tc + xoffset + yoffset;
// 	float2 tc3 = tc + xoffset - yoffset;
// 
// 	float threshold = 0.5;
// 	float dist0 = threshold - MR_SampleLod0(tex, tc0).r;
// 	float dist1 = threshold - MR_SampleLod0(tex, tc1).r;
// 	float dist2 = threshold - MR_SampleLod0(tex, tc2).r;
// 	float dist3 = threshold - MR_SampleLod0(tex, tc3).r;
// 
// 	float minDist = min(min(dist0, dist1), min(dist2, dist3));
// 	float maxDist = max(max(dist0, dist1), max(dist2, dist3));
// 
// 	float range = maxDist - minDist;
// 	float average = dist0 + dist1 + dist2 + dist3;
// 
// 	float aamask = saturate(0.5 - average / max(range * 2.0, 0.00001));
// 
// 	float2 tcAbsDelta = abs(tcdx + tcdy) * ourTextureSizes0.xy;
// 	float len = max(tcAbsDelta.x, tcAbsDelta.y);
// 	float fadeStart = 1.05;
// 	float fadeEnd = 1.3;
// 	float t = saturate((len - fadeStart) / (fadeEnd - fadeStart));
// 	float alpha = lerp(aamask, coverage, t);
// 
// 	return float4(alpha, coverage, aamask, t);
// }

float2 getTextureSize(MR_Sampler2D textureObj)
{
	uint width;
	uint height;
	textureObj.GetDimensions(width, height);
	return float2(width, height);
}

pixel float sampleOskarDistanceField(MR_Sampler2D tex, float2 tc, float padding)
{
	float2 texSize = getTextureSize(tex) / float2(padding, padding);

	float2 fwidthTS = fwidth(texSize * tc);
	float maxMovement = max(fwidthTS.x, fwidthTS.y) / 2;
	
	if( maxMovement > 0.7 ) 
		return MR_SampleLod0(tex, tc).r;

	float dist = MR_SampleLod0(tex, tc).r - 0.5 + maxMovement / 2;

	return saturate(dist / maxMovement);
}

pixel float SoftEdgeAlpha(float2 tc)
{
	float2 edgeuv = tc;
	float2 normalized = 1 - abs(lerp(-1, 1, edgeuv));
	float2 doubled = edgeuv * 2;
	float2 width = fwidth(doubled) + 0.00001;
	float2 val = saturate(normalized / width);
	return val.x * val.y;
}

pixel void doclip(float3 pos3, int index)
{
	float4 plane = ourClipPlanes[index];
	float dist = dot(plane.xyz, pos3) + plane.w;
	if (dist < 0)
		discard;
}

pixel void clipping(float4 pos4)
{
	float3 pos3 = pos4.xyz / pos4.w;

	if (ourNumClipPlanes > 0)
	{
		doclip(pos3, 0);
		doclip(pos3, 1);
		doclip(pos3, 2);
		doclip(pos3, 3);
	}
}

pixel void softStencil(float4 fragCoord)
{
	SoftTestStencil(ourStencil, fragCoord);
}

pixel void discardIfStencilContainsBits(float4 fragCoord)
{
	DiscardIfStencilContainsBits(ourStencil, fragCoord);
}

pixel float4 BlendColor(float4 color, float additive)
{
	float4 addColor = color * float4(1,1,1, additive);
	float4 blendColor = color * float4(1,1,1, (1.0 - additive));
	float4 outColor;
	outColor.rgb = addColor.rgb * addColor.a + blendColor.rgb * blendColor.a;
	outColor.a = blendColor.a;

	return outColor;
}

float4 viewportPixelToClip(float4 px)
{
	float4 t;
	t.xy = px.xy * ourUIPixelToClipScale + ourUIPixelToClipOffset * px.w;
	t.xy += ourUIViewportPos * ourUIPixelToClipScale * px.w;
	t.y = -t.y;
	t.z = px.z;
	t.w = px.w;
	return t;
}

// stub function so I don't have to resave all UI shaders. Deprecated.
pixel float4 gammaCorrect(float4 color)
{
	return color;
}

float3 UI_SphericalMap(float3 worldPosition, float3 anchorPoint, float3 focalPoint, float aZOffset)
{
	return focalPoint + normalize(worldPosition - focalPoint) * (length(anchorPoint - focalPoint) - aZOffset);
}

float3 UI_CylinderMap(float3 worldPosition, float3 anchorPoint, float3 focalPoint, float3 aCylinderDirection, float aZOffset)
{
	float3 cylinderDirection = normalize(aCylinderDirection);
	float cylinderWidth = length(anchorPoint - focalPoint) - aZOffset;

	float distanceToAxisNormalization = dot(cylinderDirection, (worldPosition - focalPoint));
	float3 finalPosition = focalPoint + cylinderDirection * distanceToAxisNormalization;

	float3 finalMappedPosition = finalPosition + normalize(worldPosition - finalPosition) * cylinderWidth;
	return finalMappedPosition;
}

pixel float4 GetColorInfo( MR_Sampler2D texture0 : MR_Texture0, float2 uv0, int renderType)
{
	if(renderType == 1) // Alpha Mask (Image)
	{
		return float4(1,1,1, MR_SampleLod0(texture0, uv0).r);
	}
	if(renderType == 2) // Distance Field (Text)
	{
		return float4(1,1,1, sampleOskarDistanceField(texture0, uv0, ourDistanceFieldMaxDistance));
	}
	if(renderType == 3) // Normal (Text)
	{
		//return float4(1,1,1, sampleOskarDistanceField(texture0, uv0, 1));
		return float4(1,1,1, MR_SampleLod0(texture0, uv0).r);
	}
	return MR_SampleLod(texture0, uv0, MR_GetLod(texture0, uv0)); // Normal sampling (Image)
}

float4 GetMatrixColumn(int transformIndex, int col)
{
	uint dataIndex = transformIndex * (4*16);
	return asfloat(ourMatrices.Load4(dataIndex + (col * 16)));
}

float4 TransformPoint(int transformIndex, float3 xyz)
{
	uint dataIndex = transformIndex * (4*16);
	float4 v0 = asfloat(ourMatrices.Load4(dataIndex + (0 * 16)));
	float4 v1 = asfloat(ourMatrices.Load4(dataIndex + (1 * 16)));
	float4 v2 = asfloat(ourMatrices.Load4(dataIndex + (2 * 16)));
	float4 v3 = asfloat(ourMatrices.Load4(dataIndex + (3 * 16)));

	return v0*xyz.x + v1*xyz.y + v2*xyz.z + v3;
}
