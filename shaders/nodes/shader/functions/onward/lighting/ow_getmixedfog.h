import "shaders/modules/graphics/onward/gi/space_fog.h"
import "shaders/modules/graphics/onward/gi/ground_fog.h"

float4 OW_GetMixedFog( float3 aFlattenedWorldPosition, float2 aPixelPosition  ) // LightingData has a float2 for this :S
{
	const float4 spaceFog = OW_SpaceFog_GetFromCoords( OW_SpaceFog_GetCoordsFromPos( aFlattenedWorldPosition ) );

	@ifndef ISNT_MAIN_PLANET
		float4 groundFog = OW_GroundFog_GetFromCoords( OW_GroundFog_GetCoordsFromPos( aFlattenedWorldPosition ) );
		return lerp( spaceFog, groundFog, MR_OnworldEnvironmentConstants.groundLerpFactor.x );
	@else
		return spaceFog;
	@endif // ISNT_MAIN_PLANET
}

float4 OW_GetMixedFog( float3 aFlattenedWorldPosition, float4 MR_FragCoord : MR_FragCoord )
{
	return OW_GetMixedFog( aFlattenedWorldPosition, MR_FragCoord.xy );
}
