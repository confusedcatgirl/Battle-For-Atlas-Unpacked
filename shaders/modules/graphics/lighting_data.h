import "shaders/MRender/MR_ViewConstants.h"
import "shaders/MRender/MR_StencilTexture.h"
import "shaders/modules/graphics/stencil.h"
import "shaders/modules/graphics/environment_constants.h"
import "shaders/modules/graphics/global_samplers.h"
import "shaders/modules/graphics/lighting_data_struct.h"

Gfx_LightingDataStruct Gfx_GetLightingDataCommon(float3 pos : Gfx_WorldSpacePos, float4 fc : MR_FragCoord, uint stencil)
{
	Gfx_LightingDataStruct ret;
	ret.worldPosition = pos;
	ret.toCamera = MR_GetPos(MR_ViewConstants.cameraToWorld).xyz - pos;
	ret.pixelPosition = fc.xy;
	ret.depth = fc.w;

@ifdef SSAO
	ret.SSAO = MR_SampleTexelLod0(SSAO, int2(fc.xy)).x;
@else
	ret.SSAO = 1.0;
@endif

@ifdef GFX_NO_INDOOR
	ret.roomIndex = 0;
	ret.isIndoor = false;
	ret.roomBits = 1;
@else
	ret.isIndoor = (stencil & Gfx_Stencil_Indoor) != 0;
	ret.roomIndex =  ret.isIndoor ? (stencil & Gfx_Stencil_RoomMask) : 0;
	ret.roomBits = 1 << uint(ret.roomIndex);
@endif

	ret.isGround = (stencil & Gfx_Stencil_IsGround) == Gfx_Stencil_IsGround;
	ret.isUnflatten = (stencil & Gfx_Stencil_Unflatten) == Gfx_Stencil_Unflatten;
	ret.isMoving = (stencil & Gfx_Stencil_IsMoving) == Gfx_Stencil_IsMoving;

	return ret;
}

Gfx_LightingDataStruct Gfx_GetLightingData(float3 pos : Gfx_WorldSpacePos, float4 fc : MR_FragCoord)
{
	uint stencil = MR_SampleStencilTexelLod0(StencilTexture, int2(fc.xy));
	Gfx_LightingDataStruct ret = Gfx_GetLightingDataCommon(pos, fc, stencil);

	return ret;
}

Gfx_LightingDataStruct Gfx_GetLightingData(float3 pos : Gfx_WorldSpacePos, float4 fc : MR_FragCoord, bool htReceiver)
{
	uint stencil = MR_SampleStencilTexelLod0(StencilTexture, int2(fc.xy));
	Gfx_LightingDataStruct ret = Gfx_GetLightingDataCommon(pos, fc, stencil);

	return ret;
}

Gfx_LightingDataStruct Gfx_GetLightingDataDummy(float3 pos, float2 fc, float depth)
{
	Gfx_LightingDataStruct ret;
	ret.worldPosition = pos;
	ret.toCamera = MR_GetPos(MR_ViewConstants.cameraToWorld).xyz - pos;
	ret.pixelPosition = fc;
	ret.depth = depth;
	ret.SSAO = 1.0;
	ret.roomIndex = 0;
	ret.isIndoor = false;
	ret.roomBits = uint(-1);
	ret.isUnflatten = false;
	return ret;
}

Gfx_LightingDataStruct Gfx_GetLightingDataDummy(float3 pos, float2 fc, float depth, bool indoor)
{
	Gfx_LightingDataStruct ret = Gfx_GetLightingDataDummy(pos, fc, depth);
	ret.isIndoor = indoor;
	ret.roomBits = indoor ? uint(-2) : uint(1);
	ret.isUnflatten = false;
	return ret;
}
