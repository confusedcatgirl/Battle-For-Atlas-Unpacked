import "shaders/nodes/Shader/Functions/Onward/OW_SampleDirFromMap.h"

float4 OW_SamplePosFromMap( in float3 aPosition, MR_Sampler2D aSampler )
{
	return OW_SampleDirFromMap( normalize( aPosition ), aSampler );
}