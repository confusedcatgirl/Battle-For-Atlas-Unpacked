import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/nodes/shader/functions/onward/planet/OW_GetPlanetPosition.h"

void OW_WaterModifiers( in float3x4 aWorldMatrixIn, in float3 aModelSpacePositionIn, out float3x4 aWorldMatrixOut, out float3 aModelSpacePositionOut )
{
	const float3 planetPosition = OW_GetPlanetPosition( );

	aWorldMatrixOut = MR_RowMatrix( float4( 1, 0, 0, planetPosition.x ) ,
	                                float4( 0, 1, 0, planetPosition.y ) ,
	                                float4( 0, 0, 1, planetPosition.z ) );

	const float3x3 temp = MR_RowMatrix( MR_GetXAxis( aWorldMatrixIn ) ,
	                                    MR_GetYAxis( aWorldMatrixIn ) ,
	                                    MR_GetZAxis( aWorldMatrixIn ) );
	aModelSpacePositionOut = normalize( MR_GetXAxis( temp ) * aModelSpacePositionIn.x +
	                                    MR_GetYAxis( temp ) * aModelSpacePositionIn.y +
	                                    MR_GetZAxis( temp ) * aModelSpacePositionIn.z ) * OW_PlanetConstants.const_WaterRadius;
}
