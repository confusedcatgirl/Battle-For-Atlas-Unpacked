constants GrassMaskConstants : OW_GrassMaskConstants
{
	float4 SamplerMask;
}

constants ForceFieldConstants : OW_ForceFieldConstants
{
	float4x4 ToWorldTransform;
	float4 constBaseAxis;
	float4x4 ToFieldTransform;
	float4 Params;
}