import "shaders/modules/graphics/onward/common/samplers_foliage.h"
import "shaders/nodes/Shader/Functions/CartesianToSphericalCoords.h"
import "shaders/nodes/Shader/Functions/Onward/OW_SampleEquirectangularTextureNoSeams.h"

float4 OW_GetGrassMaskLod0( in float3 aDirection )
{
	const float2 uv = CartesianToSphericalCoords( aDirection );
	return MR_SampleLod0( GrassMaskLookup, uv ) * GrassMaskConstants.SamplerMask;
}

float4 OW_GetGrassMask( in float3 aDirection )
{
	return OW_SampleEquirectangularTextureNoSeams( GrassMaskLookup, aDirection ) * GrassMaskConstants.SamplerMask;
}