
void CombineTilingSH(float4 aSH1, float4 aSH2,
	out float4 aSHOut, float2 aInstanceUV : Particle_InstanceUV)
{
	float2 offset = (aInstanceUV - vec2(0.5)) * float2(2,-2);
	float offsetlen = length(offset);
	float offsetlen2 = offsetlen * offsetlen;

	float xweight = aInstanceUV.x;
	float yweight = 1.0 - aInstanceUV.y;
	float zweight = 1.0 - offsetlen2;
	float avgweight = (xweight + yweight + zweight) / 3.0;

	float4 minSH = min(aSH1, aSH2);
	float4 maxSH = max(aSH1, aSH2);

	float4 simple = lerp(aSH1, aSH2, vec4(avgweight));
	aSHOut = simple;
	aSHOut.xyz = lerp(minSH.xyz, maxSH.xyz, float3(xweight, yweight, zweight));
}
