import "shaders/modules/graphics/getdepth.h"

float3 OW_Sun_DepthTest( in float3 aColor, in float3 aWorldPos, in float4 fc : MR_FragCoord )
{
	const float depth = MR_Transform( MR_ViewConstants.worldToCamera, aWorldPos ).z;
	clip( GetDepth( fc.xy ) - min( depth * 0.999, MAX_LINEAR_DEPTH ) );
	return aColor;
}
