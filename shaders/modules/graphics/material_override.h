@ifdef MATERIAL_OVERRIDE

constants MR_MaterialOverride : MR_MaterialOverride
{
	bool isEnabled;
	bool overrideDiffuse;
	bool overrideSpec;
	bool overrideOpacity;
	bool overrideRoughness;
	bool overrideMetalness;
	bool overrideAO;
	bool overrideWeatherMask;
	bool overrideUseReflection;
	bool overrideMinReflection;
	bool overrideDetailNormalMap;
	bool overrideTransmission;

	float4 replacementDiffuse;
	float4 replacementSpec;
	float replacementOpacity;
	float replacementRoughness;
	float replacementMetalness;
	float replacementAO;
	float AOscale;
	float replacementWeatherMask;
	float replacementUseReflection;
	float replacementMinReflection;
	float replacementDetailNormalMapScale;
	float replacementTransmissionOpacity;
}

void MaterialOverride_Diffuse(inout float3 color)
{
	if (MR_MaterialOverride.overrideDiffuse && MR_MaterialOverride.isEnabled)
		color = lerp(color, MR_MaterialOverride.replacementDiffuse.xyz, MR_MaterialOverride.replacementDiffuse.w);
}

void MaterialOverride_Spec(inout float3 color)
{
	if (MR_MaterialOverride.overrideSpec && MR_MaterialOverride.isEnabled)
		color = lerp(color, MR_MaterialOverride.replacementSpec.xyz, MR_MaterialOverride.replacementSpec.w);
}

void MaterialOverride_Opacity(inout float opacity)
{
	if (MR_MaterialOverride.overrideOpacity && MR_MaterialOverride.isEnabled)
		opacity = MR_MaterialOverride.replacementOpacity;
}

void MaterialOverride_Roughness(inout float roughness)
{
	if (MR_MaterialOverride.overrideRoughness && MR_MaterialOverride.isEnabled)
		roughness = MR_MaterialOverride.replacementRoughness;
}

void MaterialOverride_Metalness(inout float metalness)
{
	if (MR_MaterialOverride.overrideMetalness && MR_MaterialOverride.isEnabled)
		metalness = MR_MaterialOverride.replacementMetalness;
}

void MaterialOverride_AO(inout float ao)
{
	if (MR_MaterialOverride.isEnabled)
	{
		if (MR_MaterialOverride.overrideAO)
			ao = MR_MaterialOverride.replacementAO;
		else
			ao = 1.0 - saturate((1.0 - ao) * MR_MaterialOverride.AOscale);
	}
}

void MaterialOverride_WeatherMask(inout float weatherMask)
{
	if (MR_MaterialOverride.overrideWeatherMask && MR_MaterialOverride.isEnabled)
		weatherMask = MR_MaterialOverride.replacementWeatherMask;
}

void MaterialOverride_UseReflection(inout float useReflection)
{
	if (MR_MaterialOverride.overrideUseReflection && MR_MaterialOverride.isEnabled)
		useReflection = MR_MaterialOverride.replacementUseReflection;
}

void MaterialOverride_MinReflection(inout float minReflection)
{
	if (MR_MaterialOverride.overrideMinReflection && MR_MaterialOverride.isEnabled)
		minReflection = MR_MaterialOverride.replacementMinReflection;
}

void MaterialOverride_Transmission(inout float opacity)
{
	if (MR_MaterialOverride.overrideTransmission && MR_MaterialOverride.isEnabled)
		opacity = MR_MaterialOverride.replacementTransmissionOpacity;
}

@endif
