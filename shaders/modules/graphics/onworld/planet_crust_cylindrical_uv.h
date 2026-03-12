import "shaders/nodes/Shader/Functions/CartesianToSphericalCoords.h"
import "shaders/nodes/Shader/Functions/SphericalToCartesianCoords.h"

void OW_CalculateCylinderTBN(in float3 aSphereNormal, out float3 outSphereTangent , out float3 outSphereBitangent , out float2 outCylinderUV)
{
	aSphereNormal = aSphereNormal.x == 0.0f? normalize(float3(0.01f, aSphereNormal.y, aSphereNormal.z)) : aSphereNormal;
	outCylinderUV = CartesianToSphericalCoords(aSphereNormal);
	
	const float angleInc = 0.01f;
	const float2 sphericalCoordsPhiInc = float2(outCylinderUV.x + angleInc, outCylinderUV.y);
	outSphereTangent = normalize(SphericalToCartesianCoords(sphericalCoordsPhiInc, 1.0f) - aSphereNormal);

	const float2 sphericalCoordsThetaInc = float2(outCylinderUV.x, outCylinderUV.y + angleInc );
	outSphereBitangent = normalize(aSphereNormal - SphericalToCartesianCoords(sphericalCoordsThetaInc, 1.0f));
}