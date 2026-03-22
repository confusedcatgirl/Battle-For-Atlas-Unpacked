import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/onworld/onworld_space.h"

import "shaders/modules/graphics/onward/flattening_funcs.h"

import "shaders/modules/graphics/onward/functions/SphericalToCubicPointEx.h"

/*

           %%%    %%%
      %%%              %%%

  %%%                      %%%

 %%%                         %%%

 %%%                         %%%

 %%%                        %%%

    %%%                  %%%

          %%%     %%%
*/

global int isFlatteningActive = 1; // default

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Be aware that this function only handle the general case, everything else will be wrong
//---------------------------------------------------------------------------------------------------------------------------------------------------
float OW_SFT_ApplyUnflatteningLocal(inout float3 aLocalPos)
{
	float validity = OW_SFT_GetFlattenedFadeFactor();

	const float3 playerToCenter  = OW_ViewConstants.flatteningParams.myOriginToCenter;
	const float3 centerToPlayer  = OW_ViewConstants.flatteningParams.myCenterToOrigin;
	const float3 playerGroundPos = OW_ViewConstants.flatteningParams.myOriginGroundPos;

	float2 posHeight;

	const float3 planeNormal = centerToPlayer;
	posHeight.x = dot(planeNormal, aLocalPos);
	posHeight.y = posHeight.x - OW_ViewConstants.flatteningParams.myRadii.x;

	float3 pointOnGround = aLocalPos - planeNormal * posHeight.y;
	float3 playerToPoint = pointOnGround - playerGroundPos;
	float distToPlayerSquared = dot( playerToPoint, playerToPoint );

	float cosAngle = 1.0 - distToPlayerSquared * OW_ViewConstants.flatteningParams.myRadii.z;
	float3 translation = planeNormal * (OW_ViewConstants.flatteningParams.myMaxDelta * saturate( cosAngle ) - OW_ViewConstants.flatteningParams.myMaxDelta);
	translation += normalize( playerToPoint ) * sqrt( saturate(1.0 - cosAngle * cosAngle) ) * OW_ViewConstants.flatteningParams.myRadii.x;

	float3 upVector = normalize(playerGroundPos + translation);
	aLocalPos = upVector * posHeight.x;

	return validity;
}

float OW_SFT_ApplyUnflattening(inout float3 aWorldPos, in float3 aCenter)
{
	aWorldPos -= aCenter;
	float validity = OW_SFT_ApplyUnflatteningLocal(aWorldPos);
	aWorldPos += aCenter;

	return validity;
}

float OW_SFT_ApplyUnflattening(inout float3 aWorldPos)
{
	return OW_SFT_ApplyUnflattening(aWorldPos, OW_ViewConstants.flatteningParams.myPlanetCenter);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Preferred functions
//---------------------------------------------------------------------------------------------------------------------------------------------------

void OW_SFT_ApplyFlatteningLocal(inout float3 aPosition, in int aCategory)
{
	[branch]
	if (OW_Flattening_IsActive(OW_ViewConstants.flatteningParams, isFlatteningActive))
	{
		OW_Flattening_ApplyLocal(OW_ViewConstants.flatteningParams, OW_ViewConstants.myFlatteningPerCategory[ aCategory ], aPosition);
	}
}

void OW_SFT_ApplyAll(inout float3 aWorldPos, in int aObjectCategory)
{
	aWorldPos -= OW_ViewConstants.flatteningParams.myPlanetCenter;
	OW_SFT_ApplyFlatteningLocal(aWorldPos, aObjectCategory);
	aWorldPos += OW_ViewConstants.flatteningParams.myPlanetCenter;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Alternative functions
//---------------------------------------------------------------------------------------------------------------------------------------------------

void OW_SFT_ApplyFlatteningNoSmoothingLocal(inout float3 aPosition, in int aCategory)
{
	[branch]
	if (OW_Flattening_IsActive(OW_ViewConstants.flatteningParams, isFlatteningActive))
	{
		OW_Flattening_ApplyLocal(OW_ViewConstants.flatteningParams, OW_ViewConstants.myFlatteningPerCategory[ aCategory ], aPosition);
	}
}

void OW_SFT_ApplyOld(inout float3 aWorldPos)
{
	aWorldPos -= OW_ViewConstants.flatteningParams.myPlanetCenter;
	OW_SFT_ApplyFlatteningNoSmoothingLocal(aWorldPos, OW_DEFAULT_CATEGORY);
	aWorldPos += OW_ViewConstants.flatteningParams.myPlanetCenter;
}

void OW_SFT_ApplyTmp(inout float3 aWorldPos)
{
	aWorldPos -= OW_ViewConstants.flatteningParams.myPlanetCenter;
	OW_SFT_ApplyFlatteningNoSmoothingLocal(aWorldPos, 0);
	aWorldPos += OW_ViewConstants.flatteningParams.myPlanetCenter;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
