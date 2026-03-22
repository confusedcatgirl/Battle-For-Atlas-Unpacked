import "shaders/modules/graphics/onworld/planet_base.h"

constants : MR_ConstantBuffer1
{
	float4 constA;
	float4 constB;
	float4 constC;
}

float3 OW_GetWaterPosition( in float3 aModelSpacePosition )
{
	return normalize( constA.xyz * aModelSpacePosition.x +
	                  constB.xyz * aModelSpacePosition.y +
	                  constC.xyz * aModelSpacePosition.z ) * OW_PlanetConstants.const_WaterRadius;
}
