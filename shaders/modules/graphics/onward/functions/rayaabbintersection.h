
void OW_RayAABBIntersection( /*in float3 anOrigin,*/ in float3 aDir, in float3 aMin, in float3 aMax, out float aT0Out, out float aT1Out )
{
	float3 invR = 1.0 / aDir;
	float3 tbot = invR * (aMin /*-anOrigin*/);
	float3 ttop = invR * (aMax /*-anOrigin*/);
	float3 tmin = min(ttop, tbot);
	float3 tmax = max(ttop, tbot);
	float2 t = max(tmin.xx, tmin.yz);
	aT0Out = max(t.x, t.y);
	t = min(tmax.xx, tmax.yz);
	aT1Out = min(t.x, t.y);
}

bool OW_RayAABBIntersectionTest( /*in float3 anOrigin,*/ in float3 aDir, in float3 aMin, in float3 aMax )
{
	float2 t;
	OW_RayAABBIntersection( /*anOrigin,*/ aDir, aMin, aMax, t.x, t.y );
	return t.x <= t.y;
}