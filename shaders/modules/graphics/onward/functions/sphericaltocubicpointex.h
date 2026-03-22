
float3 OW_SphericalPointToCubicEx( in float3 aPos, in float3 aTestPos, out float3 aFacesDir )
{
	float3 absPos = abs(aPos);

	float3 absTestPos = abs(aTestPos);
	int maxc = 0;
	if (absTestPos.y > absTestPos[maxc])
		maxc = 1;
	if (absTestPos.z > absTestPos[maxc])
		maxc = 2;

	float2 st;
	if (maxc == 0)
	{
		if (aTestPos.x > 0.0f)
		{
			st.x = aPos.y;
			st.y = aPos.z;
			aPos.x = +1.0f;
		}
		else
		{
			st.x = aPos.y;
			st.y = aPos.z;
			aPos.x = -1.0f;
		}
		aPos.y = (st.x/absPos.x);
		aPos.z = (st.y/absPos.x);
		aFacesDir = float3(aPos.x,0,0);
	}
	else if (maxc == 1)
	{
		if (aTestPos.y > 0.0f)
		{
			st.x = aPos.x;
			st.y = aPos.z;
			aPos.y = +1.0f;
		}
		else
		{
			st.x = aPos.x;
			st.y = aPos.z;
			aPos.y = -1.0f;
		}
		aPos.x = (st.x/absPos.y);
		aPos.z = (st.y/absPos.y);
		aFacesDir = float3(0,aPos.y,0);
	}
	else
	{
		if (aTestPos.z > 0.0f)
		{
			st.x = aPos.x;
			st.y = aPos.y;
			aPos.z = +1.0f;
		}
		else
		{
			st.x = aPos.x;
			st.y = aPos.y;
			aPos.z = -1.0f;
		}
		aPos.x = (st.x/absPos.z);
		aPos.y = (st.y/absPos.z);
		aFacesDir = float3(0,0,aPos.z);
	}
	return aPos;
}
