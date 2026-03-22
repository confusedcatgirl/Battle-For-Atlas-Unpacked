struct OGfx_Material
{
	float roughness;
	float metalness;

	float opacity;
	float harvest;
};

float OW_Material_ArtisticToRoughnessUnclamped(in float aRoughness)
{
	return saturate(aRoughness * aRoughness);
}

float OW_Material_ArtisticToRoughness(in float aRoughness)
{
	return OW_Material_ArtisticToRoughnessUnclamped(aRoughness) * 0.95f + 0.05f;
}

float3 OW_Material_GetDominantSpecDir(float3 aR, in float3 aN, in float aRoughness, in float aAlpha)
{
	return lerp(aR, aN, aAlpha);
}

float3 OW_Material_GetDominantSpecNormalizedDir(float3 aR, in float3 aN, in float aRoughness, in float aAlpha)
{
	return normalize(OW_Material_GetDominantSpecDir(aR, aN, aRoughness, aAlpha));
}

OGfx_Material OW_Material_Init(in float aRoughness, in float aMetalness, in float aOpacity)
{
	OGfx_Material material;

	material.roughness = OW_Material_ArtisticToRoughness(aRoughness);
	material.metalness = aMetalness;

	material.opacity = aOpacity;
	material.harvest = aOpacity;

	return material;
}

OGfx_Material OW_Material_Init(in float aRoughness, in float aMetalness)
{
	return OW_Material_Init(aRoughness, aMetalness, 1.0);
}
