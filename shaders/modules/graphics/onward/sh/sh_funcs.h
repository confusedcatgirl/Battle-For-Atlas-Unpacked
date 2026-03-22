import "shaders/modules/graphics/onward/sh/sh.h"

const float SH_PI = MR_PI;

const float SH_COEFFICIENT0 = 0.50f * sqrt( 1.0f / SH_PI);
const float SH_COEFFICIENT1 = 0.50f * sqrt( 3.0f / SH_PI);
const float SH_COEFFICIENT2 = 0.50f * sqrt(15.0f / SH_PI);
const float SH_COEFFICIENT3 = 0.25f * sqrt( 5.0f / SH_PI);
const float SH_COEFFICIENT4 = 0.25f * sqrt(15.0f / SH_PI);

const float SH_COSINELOBECONSTANT0 = SH_PI;
const float SH_COSINELOBECONSTANT1 = SH_PI * (2.0f / 3.0f);
const float SH_COSINELOBECONSTANT2 = SH_PI * (1.0f / 4.0f);

Gfx_SH2Coeffs Gfx_SH3To2Order( Gfx_SH3Coeffs sh3 )
{
	Gfx_SH2Coeffs sh2;
	sh2.v0 = sh3.v0;
	return sh2;
}

Gfx_SH3Coeffs Gfx_SH3CosineLobeBands()
{
	Gfx_SH3Coeffs sh;

	sh.v0.x = SH_COSINELOBECONSTANT0;
	sh.v0.y = SH_COSINELOBECONSTANT1;
	sh.v0.z = SH_COSINELOBECONSTANT1;
	sh.v0.w = SH_COSINELOBECONSTANT1;
	sh.v1.x = SH_COSINELOBECONSTANT2;
	sh.v1.y = SH_COSINELOBECONSTANT2;
	sh.v1.z = SH_COSINELOBECONSTANT2;
	sh.v1.w = SH_COSINELOBECONSTANT2;
	sh.v2   = SH_COSINELOBECONSTANT2;

	return sh;
}

Gfx_SH3Coeffs Gfx_SH3EvalDirection(float3 dir)
{
	Gfx_SH3Coeffs sh;

	sh.v0.x = SH_COEFFICIENT0;
	sh.v0.y = SH_COEFFICIENT1 * dir.y;
	sh.v0.z = SH_COEFFICIENT1 * dir.z;
	sh.v0.w = SH_COEFFICIENT1 * dir.x;
	sh.v1.x = SH_COEFFICIENT2 * dir.x * dir.y;
	sh.v1.y = SH_COEFFICIENT2 * dir.y * dir.z;
	sh.v1.z = SH_COEFFICIENT3 * (3.0f * dir.z * dir.z - 1.0f);
	sh.v1.w = SH_COEFFICIENT2 * dir.x * dir.z;
	sh.v2   = SH_COEFFICIENT4 * (dir.x * dir.x - dir.y * dir.y);

	return sh;
}

Gfx_SH3Coeffs Gfx_SH3Multiply(Gfx_SH3Coeffs lhs, Gfx_SH3Coeffs rhs)
{
	Gfx_SH3Coeffs sh;

	sh.v0 = lhs.v0 * rhs.v0;
	sh.v1 = lhs.v1 * rhs.v1;
	sh.v2 = lhs.v2 * rhs.v2;

	return sh;
}

Gfx_SH3Coeffs Gfx_SH3EvalCosineLobe(float3 dir)
{
	return Gfx_SH3Multiply(Gfx_SH3CosineLobeBands(), Gfx_SH3EvalDirection(dir));
}

float Gfx_SH3Dot(Gfx_SH3Coeffs lhs, Gfx_SH3Coeffs rhs)
{
	return mad( lhs.v2, rhs.v2, dot(lhs.v0, rhs.v0) + dot(lhs.v1, rhs.v1) );
}

float3 Gfx_SH3Dot(Gfx_SH3Coeffs shR, Gfx_SH3Coeffs shG, Gfx_SH3Coeffs shB, Gfx_SH3Coeffs shDir)
{
	float3 result = 0.0f;
	result.r = Gfx_SH3Dot(shR, shDir);
	result.g = Gfx_SH3Dot(shG, shDir);
	result.b = Gfx_SH3Dot(shB, shDir);
	return result;
}
