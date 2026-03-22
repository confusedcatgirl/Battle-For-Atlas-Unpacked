import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/onworld/planet_base.h"
import "shaders/modules/graphics/onworld/onworld_space.h"

import "shaders/modules/graphics/onworld/functions/MiePhase.h"
import "shaders/modules/graphics/onworld/functions/RayPhase.h"

struct OW_PositionContext
{
	float3 myAbsolutePos;

	float3 mySphereAbsolutePos;
	float3 mySphereRelativePos;
	float3 mySphereDir;
	float3 myGroundDir;

	float myHeightFromCenter;
	float myHeightFromGround;

	float mySpaceFactor;
	bool  myIsPosInSpace;
};

float3 OW_SFT_ApplyUnflattening4(in float3 aLocalPos, out float3 anUpVector, out float2 aPosHeight)
{
	const float3 playerToCenter  = OW_ViewConstants.flatteningParams.myOriginToCenter;
	const float3 centerToPlayer  = OW_ViewConstants.flatteningParams.myCenterToOrigin;
	const float3 playerGroundPos = OW_ViewConstants.flatteningParams.myOriginGroundPos;

	const float3 planeNormal = centerToPlayer;
	aPosHeight.x = dot(planeNormal, aLocalPos);
	aPosHeight.y = aPosHeight.x - OW_ViewConstants.flatteningParams.myRadii.x;

	float3 pointOnGround = aLocalPos - planeNormal * aPosHeight.y;
	float3 playerToPoint = pointOnGround - playerGroundPos;
	float distToPlayerSquared = dot( playerToPoint, playerToPoint );

	float cosAngle = 1.0 - distToPlayerSquared * OW_ViewConstants.flatteningParams.myRadii.z;
	float3 translation = planeNormal * (OW_ViewConstants.flatteningParams.myMaxDelta * saturate( cosAngle ) - OW_ViewConstants.flatteningParams.myMaxDelta);
	translation += normalize( playerToPoint ) * sqrt( saturate(1.0 - cosAngle * cosAngle) ) * OW_ViewConstants.flatteningParams.myRadii.x;

	anUpVector = normalize(playerGroundPos + translation);
	return anUpVector * aPosHeight.x;
}

void OW_InitPositionContext(out OW_PositionContext aCtx, in float3 aWorldPosition, in float4 aGroundSphere, bool aForceSpaceFactor, bool aIsUnflatten)
{
	aCtx.myAbsolutePos = aWorldPosition;

@ifndef ISNT_MAIN_PLANET
	const float flatFactor = aIsUnflatten ? 0.0f : OW_SFT_GetFlatteningFactor();

	if (flatFactor > 0.001f)
	{
		float2 heights;

		aCtx.mySphereRelativePos = aWorldPosition - OW_Planet_GetPosition();
		aCtx.mySphereRelativePos = OW_SFT_ApplyUnflattening4( aCtx.mySphereRelativePos, aCtx.mySphereDir, heights );
		aCtx.mySphereAbsolutePos = aCtx.mySphereRelativePos + OW_Planet_GetPosition();
		aCtx.myGroundDir = OW_ViewConstants.flatteningParams.myCenterToOrigin;

		aCtx.myHeightFromCenter = heights.x;
		aCtx.myHeightFromGround = heights.y;

		const float3 radii = OW_Space_GetInfluenceRadii( OW_Planet_GetBaseGroundRadius( ) );
		aCtx.mySpaceFactor = ( aForceSpaceFactor ? 1.0f : OW_Space_GetInfluenceFromRadii(radii, aCtx.myHeightFromCenter) );
	}
	else
@endif // ISNT_MAIN_PLANET
	{
		aCtx.mySphereAbsolutePos = aWorldPosition;
		aCtx.mySphereRelativePos = aWorldPosition - aGroundSphere.xyz;

		aCtx.myHeightFromCenter = length(aCtx.mySphereRelativePos);
		aCtx.myHeightFromGround = aCtx.myHeightFromCenter - aGroundSphere.w;

		aCtx.mySphereDir = aCtx.mySphereRelativePos * rcp(aCtx.myHeightFromCenter);
		aCtx.myGroundDir = aCtx.mySphereDir;

		const float3 radii = OW_Space_GetInfluenceRadii( aGroundSphere.w );
		aCtx.mySpaceFactor = ( aForceSpaceFactor ? 1.0f : OW_Space_GetInfluenceFromRadii(radii, aCtx.myHeightFromCenter) );
	}

	aCtx.myIsPosInSpace = aCtx.mySpaceFactor >= 0.5f;
}
