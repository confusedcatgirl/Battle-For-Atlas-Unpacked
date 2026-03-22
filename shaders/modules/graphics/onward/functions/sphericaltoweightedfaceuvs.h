
void OW_SphericalToWeightedFaceUVs(in float3 aDir, out float4 aUV[3], out float3 aWeightPos, out float3 aWeightNeg)
{
	const float3 absDir = abs( aDir );

	int maxc = 0;
	if (absDir.y > absDir.x)
		maxc = 1;
	if (absDir.z > absDir[maxc])
		maxc = 2;

	aWeightPos = (float3)(0.0f);
	aWeightNeg = (float3)(0.0f);

	if (maxc == 0)
	{
		if (aDir.x > 0.0f)
			aWeightPos.x = 1.0f;
		else
			aWeightNeg.x = 1.0f;
	}
	else if (maxc == 1)
	{
		if (aDir.y > 0.0f)
			aWeightPos.y = 1.0f;
		else
			aWeightNeg.y = 1.0f;
	}
	else
	{
		if (aDir.z > 0.0f)
			aWeightPos.z = 1.0f;
		else
			aWeightNeg.z = 1.0f;
	}

	aUV[0].xy = float2( -aDir.z/absDir.x, -aDir.y/absDir.x );
	aUV[0].zw = float2(  aDir.z/absDir.x, -aDir.y/absDir.x );
	aUV[1].xy = float2(  aDir.x/absDir.y,  aDir.z/absDir.y );
	aUV[1].zw = float2(  aDir.x/absDir.y, -aDir.z/absDir.y );
	aUV[2].xy = float2(  aDir.x/absDir.z, -aDir.y/absDir.z );
	aUV[2].zw = float2( -aDir.x/absDir.z, -aDir.y/absDir.z );

	aUV[0] = aUV[0] * 0.5f + 0.5f;
	aUV[1] = aUV[1] * 0.5f + 0.5f;
	aUV[2] = aUV[2] * 0.5f + 0.5f;
}
