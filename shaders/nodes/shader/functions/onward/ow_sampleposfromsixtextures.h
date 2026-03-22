import "shaders/nodes/Shader/Functions/Onward/OW_SampleDirFromSixTextures.h"

float4 OW_SamplePosFromSixTextures( in float3 aPosition,
	MR_Sampler2D aSamplerPosX,
	MR_Sampler2D aSamplerNegX,
	MR_Sampler2D aSamplerPosY,
	MR_Sampler2D aSamplerNegY,
	MR_Sampler2D aSamplerPosZ,
	MR_Sampler2D aSamplerNegZ )
{
	return OW_SampleDirFromSixTextures( normalize( aPosition ), aSamplerPosX, aSamplerNegX, aSamplerPosY, aSamplerNegY, aSamplerPosZ, aSamplerNegZ );
}