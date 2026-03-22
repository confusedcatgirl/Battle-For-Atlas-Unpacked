import "shaders/modules/graphics/onward/gi/space_fog.h"
import "shaders/modules/graphics/onworld/env_constants.h"
import "shaders/modules/graphics/onward/gi/space_fog_far.h"

float3 OW_GetSpaceLightVolumeFar( float3 aWorldPosition )
{
	float3 viewDir = normalize( aWorldPosition - MR_GetPos( MR_ViewConstants.cameraToWorld ).xyz );
	return OW_SpaceFog_GetFarLighting( aWorldPosition, viewDir );
}