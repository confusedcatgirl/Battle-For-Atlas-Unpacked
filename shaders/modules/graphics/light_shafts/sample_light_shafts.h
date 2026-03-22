
import "shaders/MRender/MR_ViewConstants.h"

MR_Sampler3D LightShaftsVolume : MR_LightShaftsVolume
{
	filter = linear point,
	wrap = border,
	border = 0 0 0 0,
};

constants LightShaftsVolumeConstants : MR_LightShaftsVolumeConstants
{
	float EndDistance;
	uint NumVolumeSlices;
}

float SampleVolumeLightShaftsRaw( float2 texCoord, float aDistance )
{
	float sliceDistance = LightShaftsVolumeConstants.EndDistance / LightShaftsVolumeConstants.NumVolumeSlices;

	float sampleDepth = ( aDistance - sliceDistance ) / ( LightShaftsVolumeConstants.EndDistance );
	sampleDepth = min( sampleDepth, 1 - 1.0 / LightShaftsVolumeConstants.NumVolumeSlices );

	return MR_SampleLod0( LightShaftsVolume, float3( texCoord, sampleDepth ) ).r;
}
