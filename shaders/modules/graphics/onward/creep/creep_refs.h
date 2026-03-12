import "shaders/modules/graphics/onward/creep/creep_defs.h"

constants OW_Creep_Constants : OW_CreepConstants
{
	float3   myPlanetPosition;
	int      myNumberOfZones;

	float    myMinPlanetRadius;
	float    myMaxPlanetRadius;

	float    myFullyCoveredFactor;
	float    myFullyExposedFactor;
}

MR_StructuredBuffer< OW_Creep_Zone > OW_Creep_Buffer : OW_CreepBuffer;

MR_SamplerCube OW_Creep_Mask_Lookup : OW_CreepMaskLookup;
MR_SamplerCube OW_Creep_Zone_Lookup : OW_CreepZoneLookup { filter = point };
MR_SamplerCube OW_Creep_Base_Lookup : OW_CreepBaseLookup { filter = point };

MR_Sampler2D OW_Creep_Projector : OW_CreepProjector { filter = linear, wrap = repeat };

int4 OW_Creep_GetZones(in float3 aPos)
{
	return (int4)(MR_SampleLod0(OW_Creep_Zone_Lookup, aPos) * 255.0);
}
