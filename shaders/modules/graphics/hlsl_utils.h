import "shaders/MRender/MR_ViewConstants.h"

float3 MR_RotateOnXAxis(float3 anOriginalVector, float anAngle)
{
	return float3(anOriginalVector.x,
					anOriginalVector.y*cos(anAngle) - anOriginalVector.z*sin(anAngle), 
					anOriginalVector.y*sin(anAngle) + anOriginalVector.z*cos(anAngle));
}

float3 MR_RotateOnYAxis(float3 anOriginalVector, float anAngle)
{
	return float3(anOriginalVector.x*cos(anAngle) + anOriginalVector.z*sin(anAngle), 
					anOriginalVector.y,
					-anOriginalVector.x*sin(anAngle) + anOriginalVector.z*cos(anAngle));
					
}

float3 MR_RotateOnZAxis(float3 anOriginalVector, float anAngle)
{
	return float3(anOriginalVector.x*cos(anAngle) - anOriginalVector.y*sin(anAngle), 
					anOriginalVector.x*sin(anAngle) + anOriginalVector.y*cos(anAngle),
					anOriginalVector.z);
}

// 6.2 The normalized matrix for rotation about an arbitrary line
// http://inside.mines.edu/fs_home/gmurray/ArbitraryAxisRotation/
float3 MR_RotateAroundAxis(float3 anOriginalVector, float3 aRotAxis, float anAngle)
{
    const float u = aRotAxis.x;
    const float v = aRotAxis.y;
    const float w = aRotAxis.z;

    float sin_t = 0.0f;
    float cos_t = 0.0f;
    sincos(anAngle, sin_t, cos_t);

    const float one_cos_T = 1.0f - cos_t;

    const float vv = v*v;
    const float uu = u*u;
    const float uv = u*v;
    const float vw = v*w;
    const float ww = w*w;
    const float uw = u*w;

    float3x4 rotMatrix = float3x4(
			uu+(vv+ww)*cos_t,     uv*one_cos_T-w*sin_t, uw*one_cos_T+v*sin_t, 0,
            uv*one_cos_T+w*sin_t, vv+(uu+ww)*cos_t,     vw*one_cos_T-u*sin_t, 0,
            uw*one_cos_T-v*sin_t, vw*one_cos_T+u*sin_t, ww+(uu+vv)*cos_t,     0);

    return MR_Transform(rotMatrix, anOriginalVector);
}

float3 MR_RotateAroundAxis(float3 anOriginalVector, float3 aRotAxis, float anCos, float anSin)
{
    const float u = aRotAxis.x;
    const float v = aRotAxis.y;
    const float w = aRotAxis.z;

    float sin_t = anSin;
    float cos_t = anCos;
    
    const float one_cos_T = 1.0f - cos_t;

    const float vv = v*v;
    const float uu = u*u;
    const float uv = u*v;
    const float vw = v*w;
    const float ww = w*w;
    const float uw = u*w;

    float3x4 rotMatrix = float3x4(
			uu+(vv+ww)*cos_t,     uv*one_cos_T-w*sin_t, uw*one_cos_T+v*sin_t, 0,
            uv*one_cos_T+w*sin_t, vv+(uu+ww)*cos_t,     vw*one_cos_T-u*sin_t, 0,
            uw*one_cos_T-v*sin_t, vw*one_cos_T+u*sin_t, ww+(uu+vv)*cos_t,     0);

    return MR_Transform(rotMatrix, anOriginalVector);
}

float MR_AngleBetweenVectors(float3 Vector1, float3 Vector2)
{
	float lengthVector1 = length( Vector1 );
	float lengthVector2 = length( Vector2 );
	
	if( lengthVector1 == 0 || lengthVector1 == 0)
	{
		return 0;
	}

	 float dotProd = dot(Vector1, Vector2);
	 dotProd = dotProd/( lengthVector1 * lengthVector2 );
	 float myAcos = acos(dotProd);

	 return myAcos;
}

float MR_Remap(float aValue,float aLow1,float aHigh1,float aLow2,float aHigh2)
{
	return aLow2 + (aValue - aLow1) * (aHigh2 - aLow2) / (aHigh1 - aLow1);
}