import "shaders/modules/graphics/environment.h"
import "shaders/nodes/Shader/Functions/Lighting/GI.h"

constants : MR_ConstantBuffer0
{
	float MaxRange;
	float DistributionGamma;
	float InvDistributionGamma;
	float DistributionGamma3D;
	float InvDistributionGamma3D;
	float GI;
	float SunScale;
	float Alpha;
	float IndoorAlpha;
	float ThisFrameAlpha;
	float Scattering;
	float NoiseStrength;
	float Near;
	float Height;
	float GroundFogHeight;
	float GroundFogAlpha;
	uint GITileSizeShift;
	uint GITileShiftY;
	uint NumVolumes;
	uint NumVolumeTilesPerLine;
	uint NumVolumeTilesPerSlice;
	
	float4x4 LastFrameMatrix;

	float4 CenterPosition;
	float4 LightVolumeSize;
	float4 GIVolumeSize;
	float4 Jitter;
	float4 ParameterDistances[32];
	float4 Parameters[128];

	float2 myFogWindTopDownCenter;
	float myFogWindInvSize;
	float myNominalWindSpeedInv;
}

MR_ByteBuffer TileData2D : MR_Buffer9;
