import "shaders/modules/graphics/onward/gi/gi_sampling.h"
import "shaders/modules/graphics/onward/gi/space_fog.h"

float3 OW_Space_GI( float3 aDirection, float3 aWorldPosition )
{
	float3 galaxyGI = OW_GI_SampleFallback( OW_GIConstants.myGalaxy.myFallback, aDirection );
@ifndef OGI_IS_BAKING_GALAXY
	galaxyGI = OW_GI_SampleGalaxy( aWorldPosition, aDirection, galaxyGI );
@endif // OGI_IS_BAKING_GALAXY

	return galaxyGI;
}
