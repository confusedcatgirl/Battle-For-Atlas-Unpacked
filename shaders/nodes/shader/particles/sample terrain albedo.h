import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/nodes/Shader/Particles/Get Sample Position.h"
import "shaders/nodes/shader/functions/onward/OW_GetTerrainLowResAlbedo.h"

float3 SampleParticlesTerrainAlbedo( )
{
	return OW_GetTerrainLowResAlbedo( GetParticlesSamplePosition( ) - OW_Planet_GetBaseGroundSphere( ).xyz );
}
