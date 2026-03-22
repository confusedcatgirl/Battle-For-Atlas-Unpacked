import "shaders/modules/graphics/environment.h"
import "shaders/modules/graphics/reflection.h"

void SSR_FriendlyReflection(
	out float4 aLightOut,
	float3 aNormal,
	float3 Gfx_WorldSpacePos : Gfx_WorldSpacePos,
	float4 MR_FragCoord : MR_FragCoord
)
{
	aLightOut = vec4(0);

@ifndef GFX_NO_REFLECTIONS
	float2 clip = MR_PixelToClip(MR_FragCoord.xy);
	float2 uv = vec2(0.5) + clip * 0.5;
	float3 col = MR_SampleLod0(Reflection, uv).xyz;

	aLightOut = float4(col, 1);
@endif
}
