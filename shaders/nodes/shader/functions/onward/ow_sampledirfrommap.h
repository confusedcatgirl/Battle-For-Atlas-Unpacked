import "shaders/nodes/Shader/Functions/CartesianToSphericalCoords.h"

float4 OW_SampleDirFromMap( in float3 aDirection, MR_Sampler2D aSampler )
{
	const float2 uv = CartesianToSphericalCoords( aDirection );
	return MR_Sample( aSampler, uv );
}