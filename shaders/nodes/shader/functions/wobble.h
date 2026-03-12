import "shaders/modules/graphics/environment.h"
float3 Wobble( float3 position, float scale, float frequency, float3 strength, float weight )
{
	float3 sinTime = sin( position / scale + Gfx_Environment.time * frequency );
	float3 offset = float3( sinTime.y + sinTime.z, sinTime.x + sinTime.z, sinTime.x + sinTime.y );
	offset *= strength;
	offset *= weight;
	return position + offset;
}
