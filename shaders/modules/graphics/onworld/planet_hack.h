import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/MRender/MR_ViewConstants.h"

float3 OW_Planet_GIEmissionModifier(in float3 aPosition, in float3 aEmission)
{
@ifdef OGI_IS_BAKING
    aPosition = MR_Transform( OW_Planet_GetWorldTransform(), float4( aPosition.xyz, 1.0 ) ).xyz;
    const float radius = OW_Planet_GetMinTerrainRadius();
    float scale = radius / max( distance( aPosition, MR_ViewConstants.LODCameraPosition.xyz ) - radius, 1.0 );
    return aEmission * saturate( scale * 16.0 / length( aEmission ) );
@else
    return aEmission;
@endif // OGI_IS_BAKING
}