import "shaders/modules/graphics/onward/functions/SphericalToWeightedFaceUVs.h"

float4 OW_SampleDirFromSixTextures( in float3 aDirection,
	MR_Sampler2D aSamplerPosX,
	MR_Sampler2D aSamplerNegX,
	MR_Sampler2D aSamplerPosY,
	MR_Sampler2D aSamplerNegY,
	MR_Sampler2D aSamplerPosZ,
	MR_Sampler2D aSamplerNegZ )
{
	float4 uv[ 3 ];
	float3 weightPos;
	float3 weightNeg;

	OW_SphericalToWeightedFaceUVs( aDirection, uv, weightPos, weightNeg );

	return MR_Sample( aSamplerPosX, uv[0].xy ) * weightPos.x +
	       MR_Sample( aSamplerNegX, uv[0].zw ) * weightNeg.x +
	       MR_Sample( aSamplerPosY, uv[1].xy ) * weightPos.y +
	       MR_Sample( aSamplerNegY, uv[1].zw ) * weightNeg.y +
	       MR_Sample( aSamplerPosZ, uv[2].xy ) * weightPos.z +
	       MR_Sample( aSamplerNegZ, uv[2].zw ) * weightNeg.z;
}