import "shaders/modules/graphics/onward/sh/sh.h"

MR_SamplerCube PlanetGndColorGICubemap : OW_GroundGICubemaps0 { anisotropy = 0 };
MR_SamplerCube PlanetSkyColorGICubemap : OW_GroundGICubemaps1 { anisotropy = 0 };

MR_Sampler3D PlanetGIVolume_PX : OW_PlanetGIVolumes0 { filter = linear point, wrap = clamp };
MR_Sampler3D PlanetGIVolume_NX : OW_PlanetGIVolumes1 { filter = linear point, wrap = clamp };
MR_Sampler3D PlanetGIVolume_PY : OW_PlanetGIVolumes2 { filter = linear point, wrap = clamp };
@ifndef OGI_MERGE_VOLUME_BASES
	MR_Sampler3D PlanetGIVolume_NY : OW_PlanetGIVolumes3 { filter = linear point, wrap = clamp };
	MR_Sampler3D PlanetGIVolume_PZ : OW_PlanetGIVolumes4 { filter = linear point, wrap = clamp };
	MR_Sampler3D PlanetGIVolume_NZ : OW_PlanetGIVolumes5 { filter = linear point, wrap = clamp };
@endif // OGI_MERGE_VOLUME_BASES

MR_Sampler3D GalaxyGIVolume_PX : OW_GalaxyGIVolumes0 { filter = linear point, wrap = clamp };
MR_Sampler3D GalaxyGIVolume_NX : OW_GalaxyGIVolumes1 { filter = linear point, wrap = clamp };
MR_Sampler3D GalaxyGIVolume_PY : OW_GalaxyGIVolumes2 { filter = linear point, wrap = clamp };
@ifndef OGI_MERGE_VOLUME_BASES
	MR_Sampler3D GalaxyGIVolume_NY : OW_GalaxyGIVolumes3 { filter = linear point, wrap = clamp };
	MR_Sampler3D GalaxyGIVolume_PZ : OW_GalaxyGIVolumes4 { filter = linear point, wrap = clamp };
	MR_Sampler3D GalaxyGIVolume_NZ : OW_GalaxyGIVolumes5 { filter = linear point, wrap = clamp };
@endif // OGI_MERGE_VOLUME_BASES

struct OW_GIVolumeConstants
{
	float4 myVolumeRange;
	float3 myVolumeBias;
	float3 myVolumeInvSize;
	float  myStrength;
	float4 myFallback[6];
};

constants OW_GIConstants : OW_GIConstants
{
	OW_GIVolumeConstants myGalaxy;
	OW_GIVolumeConstants myPlanet;

	Gfx_SH3Coeffs mySkySH3CoeffsR;
	float3 myPadding1;
	Gfx_SH3Coeffs mySkySH3CoeffsG;
	float3 myPadding2;
	Gfx_SH3Coeffs mySkySH3CoeffsB;
	float3 myPadding3;
	Gfx_SH3Coeffs mySkySH3CoeffsA;
	float3 myPadding4;
}
