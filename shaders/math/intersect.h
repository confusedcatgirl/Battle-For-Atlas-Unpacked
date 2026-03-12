const float INTERSECT_FLT_MAX = 3.40282347E+38F;

float Ray_PlaneIntersect(float4 aPlane, float3 anOrigin, float3 aDirection)
{
	const float3 planeNormal = aPlane.xyz;
	const float dotND = dot(planeNormal, aDirection);
	if (abs(dotND) < 0.0001f)
		return INTERSECT_FLT_MAX;

	const float3 pointInPlane = planeNormal * -aPlane.w;
	const float3 w = pointInPlane - anOrigin;
	return dot(planeNormal, w) / dotND;
}

float DistanceToPlane(float4 aPlane, float3 aPoint)
{
	return dot(aPlane.xyz, aPoint) + aPlane.w;
}

// Check url below for correct frustum culling checking
// http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
bool IntersectsBox(float4 somePlanes[6], float3 minBox, float3 maxBox)
{
	for (uint i = 0; i < 6; ++i)
	{
		int outTest = 0;
		outTest += DistanceToPlane(somePlanes[i], float3(minBox.x, minBox.y, minBox.z)) < 0.0f? 1 : 0;
		outTest += DistanceToPlane(somePlanes[i], float3(minBox.x, minBox.y, maxBox.z)) < 0.0f? 1 : 0;
		outTest += DistanceToPlane(somePlanes[i], float3(minBox.x, maxBox.y, minBox.z)) < 0.0f? 1 : 0;
		outTest += DistanceToPlane(somePlanes[i], float3(minBox.x, maxBox.y, maxBox.z)) < 0.0f? 1 : 0;
		outTest += DistanceToPlane(somePlanes[i], float3(maxBox.x, minBox.y, minBox.z)) < 0.0f? 1 : 0;
		outTest += DistanceToPlane(somePlanes[i], float3(maxBox.x, minBox.y, maxBox.z)) < 0.0f? 1 : 0;
		outTest += DistanceToPlane(somePlanes[i], float3(maxBox.x, maxBox.y, minBox.z)) < 0.0f? 1 : 0;
		outTest += DistanceToPlane(somePlanes[i], float3(maxBox.x, maxBox.y, maxBox.z)) < 0.0f? 1 : 0;

		if (outTest == 8)
			return false;
	}

	return true;
}

const float CULLING_EPSILON = 0.00001;
bool IntersectsSphere(float4 somePlanes[6], float3 aSphereCentre, float aRadius)
{
	for (uint i = 0; i < 6; ++i)
	{
		float dist = DistanceToPlane(somePlanes[i], aSphereCentre);
		if (dist < -(aRadius + CULLING_EPSILON)) // If we are outside any plane, we are outside the frustum
			return false;
	}
	
	return true;
}

bool PlaneIntersectsSphere(out bool aCenterOutside, float4 aPlane, float3 aSphereCentre, float aRadius)
{
	aCenterOutside = false;
	
	float dist = DistanceToPlane(aPlane, aSphereCentre);
	if (dist < -(aRadius + CULLING_EPSILON))
		return false;
	else if (dist < -CULLING_EPSILON)
		aCenterOutside = true;
	
	return true;
}

bool LineIntersectsSphere(float3 aStartPoint, float3 aEndPoint, float3 aSpherePosition, float aSphereRadius)
{
	float fSphereRadiusSqr = aSphereRadius * aSphereRadius;
	float3 lineStartToSphereVec = aSpherePosition - aStartPoint;
	float3 lineEndToSphereVec = aSpherePosition - aEndPoint;
	float fSphereToLineStartDistSqr = dot(lineStartToSphereVec, lineStartToSphereVec);
	float fSphereToLineEndDistSqr = dot(lineEndToSphereVec, lineEndToSphereVec);
	
	bool bLineStartInsideSphere = fSphereToLineStartDistSqr < fSphereRadiusSqr;
	bool bLineEndInsideSphere = fSphereToLineEndDistSqr < fSphereRadiusSqr;
	if (bLineStartInsideSphere || bLineEndInsideSphere)
		return true;

	float3 lineVec = aEndPoint - aStartPoint;
	float fLineLengthSqr = dot(lineVec, lineVec);
	if (fLineLengthSqr < CULLING_EPSILON)
		return false;
		
	float u = dot(lineStartToSphereVec, lineVec) / fLineLengthSqr;
	if (u >= 0.0 && u <= 1.0)
	{
		float3 closestToSpherePos = aStartPoint + lineVec * u;

		float3 spherePosToLinePos = closestToSpherePos - aSpherePosition;
		float distSqr = dot(spherePosToLinePos, spherePosToLinePos);
		if (distSqr < fSphereRadiusSqr)
			return true;
	}
	
	return false;
}

bool TileIntersectsSphere(float4 somePlanes[6], float3 frustumEdgesStart[4], float3 frustumEdgesEnd[4], float3 aSphereCentre, float aRadius)
{
	bool centerOutside[6];
	for (uint i = 0; i < 6; ++i)
	{
		if (!PlaneIntersectsSphere(centerOutside[i], somePlanes[i], aSphereCentre, aRadius))
			return false;
	}

	bool intersect = true;
	if (centerOutside[2]) // Outside left
	{
		if (centerOutside[4]) // Outside top
			intersect = LineIntersectsSphere(frustumEdgesStart[0], frustumEdgesEnd[0], aSphereCentre, aRadius);
		else if (centerOutside[5]) // Outside bottom
			intersect = LineIntersectsSphere(frustumEdgesStart[3], frustumEdgesEnd[3], aSphereCentre, aRadius);
	}
	else if (centerOutside[3]) // Outside right
	{
		if (centerOutside[4]) // Outside top
			intersect = LineIntersectsSphere(frustumEdgesStart[1], frustumEdgesEnd[1], aSphereCentre, aRadius);
		else if (centerOutside[5]) // Outside bottom
			intersect = LineIntersectsSphere(frustumEdgesStart[2], frustumEdgesEnd[2], aSphereCentre, aRadius);
	}
	
	return intersect;
}
