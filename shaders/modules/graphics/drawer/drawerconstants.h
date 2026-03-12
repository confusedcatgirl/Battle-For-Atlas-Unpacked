constants : MR_ConstantBuffer0
{
	float2 ourUIPixelToClipScale;
	float2 ourUIPixelToClipOffset;
	float2 ourUIViewportPos;
	int ourDistanceFieldMaxDistance;
	float ourUnused;
}

MR_ByteBuffer ourMatrices : MR_Buffer0;

constants : MR_ConstantBuffer1
{
	uint ourStencil;
	uint ourNumClipPlanes;
	float ourUsesPerspectiveCorrect;
	uint ourTransformMatrixOverride;

	float4 ourClipPlanes[4];
}
