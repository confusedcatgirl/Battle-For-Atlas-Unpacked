global bool ourForceDisableScreenspaceShadows = false;

@ifndef GFX_NO_SHADOWS

MR_Sampler2DArrayCmp ShadowMap : MR_ShadowMap
{
	filter = linear point,
	wrap = clamp,
	compare = >,
	mutable = true
};

@ifdef GFX_NVIDIA_SHADOWS

global float ourNVIDIASampleMask = 1.0;
global float ourNVIDIASampleMaps = 2.0;

MR_Sampler2D NVIDIAShadowMask : MR_NVIDIAShadowMask
{
	filter = linear point,
	wrap = clamp,
};

@endif

constants ShadowConstants : MR_ShadowConstants
{
	float4 fadingPlane;
	float fadingRange;
	float filterOffset;
	float pixelSize;
	float NVIDIAShadowSampleType;
	float4 sampleRotation;
	float4x4 shadowMapWorldToClipZBiased[4];
}

@ifndef GFX_LOW_QUALITY_SHADOWS
	MR_Sampler2D ShadowMapNoise
	{
		texture = "textures/randomrotate.tga",
		filter = point,
	};
	
	float2 GetRotateVec(float2 aScreenSpaceCoord)
	{
		float2 vec = MR_SampleLod0(ShadowMapNoise, aScreenSpaceCoord / 128.0).xy * 2.0 - 1.0;
		vec	= MR_Transform(float2x2(ShadowConstants.sampleRotation.xy, ShadowConstants.sampleRotation.zw), vec);
		return vec;
	}
@else
	float2 GetRotateVec(float2 aScreenSpaceCoord)
	{
		return float2(0.0, 0.0);
	}
@endif

@endif
