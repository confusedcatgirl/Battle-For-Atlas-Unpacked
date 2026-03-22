@ifdef MATERIAL_OVERRIDE
import "shaders/modules/graphics/material_override.h"
@endif

void MaterialOverride(inout float3 diffuse, inout float3 spec, inout float roughness, inout float ao, inout float useReflection)
{
@ifdef MATERIAL_OVERRIDE
	MaterialOverride_Diffuse(diffuse);
	MaterialOverride_Spec(spec);
	MaterialOverride_Roughness(roughness);
	MaterialOverride_AO(ao);
	MaterialOverride_UseReflection(useReflection);
@endif
}
