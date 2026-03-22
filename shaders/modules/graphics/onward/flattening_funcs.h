void OW_Flattening_ApplyLocal(in OGFX_FlatteningParams someParams, in float4 someFactors, inout float3 aPosition)
{
	const float3 playerToCenter  = someParams.myOriginToCenter;
	const float3 centerToPlayer  = someParams.myCenterToOrigin;
	const float3 playerGroundPos = someParams.myOriginGroundPos;

	// compute.... normalize and get length (vertex elevation above ground)
	float pointHeight = length(aPosition);
	const float3 centerToPoint = aPosition * rcp(pointHeight);
	pointHeight -= someParams.myRadii.y;
	pointHeight *= someFactors.z;
	pointHeight -= someParams.myRadii.w;

	const float cosAngle = dot(centerToPlayer, centerToPoint);
	const float3 groundPos = centerToPoint * someParams.myRadii.x;
	float3 playerToPoint = groundPos - playerGroundPos;
	float distanceToPlayer = length(playerToPoint);

@ifdef NO_CURVATURE
	const float flatteningFactor = someFactors.y;
@else
	const float flatteningFactor = someFactors.y - someFactors.w * exp(cosAngle * someParams.myCurvatureParam1);
@endif // NO_CURVATURE

	// project the smoothed point on the flat plane
	float3 direction = lerp(centerToPoint, centerToPlayer, flatteningFactor);
	float3 translation = direction * (someParams.myMaxDelta - someParams.myMaxDelta * cosAngle);

	translation += playerToPoint;
	translation = normalize(translation) * distanceToPlayer;
	translation += direction * pointHeight;

	aPosition = lerp(aPosition, playerGroundPos + translation, step(0.0, cosAngle));
}

void OW_Flattening_ApplyLocal(in OGFX_FlatteningParams someParams, inout float3 aPosition)
{
	OW_Flattening_ApplyLocal(someParams, someParams.myFactors, aPosition);
}

bool OW_Flattening_IsActive(in OGFX_FlatteningParams someParams, in int aIsFlatteningActive)
{
	return (someParams.myIsFlatteningActive * aIsFlatteningActive) != 0;
}

bool OW_Flattening_IsActive(in OGFX_FlatteningParams someParams)
{
	return OW_Flattening_IsActive(someParams, 1);
}

void OW_Flattening_Apply(in OGFX_FlatteningParams someParams, inout float3 aPosition)
{
	aPosition -= someParams.myPlanetCenter;
	OW_Flattening_ApplyLocal(someParams, aPosition);
	aPosition += someParams.myPlanetCenter;
}