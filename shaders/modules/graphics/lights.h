import "shaders/modules/graphics/lighting_data_struct.h"
import "shaders/modules/graphics/lights_common.h"
import "shaders/modules/graphics/lighting_constants.h"

//MR_Sampler2DArray SpotlightTexture : MR_SpotlightTexture { wrap = border };
//MR_Sampler1DArray AngularLightTexture : MR_AngularLightTexture { wrap = border };

constants SpotlightTextureConstants : MR_SpotlightTextureConstants
{
	float4 textureData[32]; // really unbounded size, but that causes errors in this case
}

float3 GetProjectedTextureEmission(Gfx_Light aLight, float3 aWorldSpacePos)
{
	uint dataIndex = aLight.projectedTextureDataIndex * 4;
    float4 v2 = SpotlightTextureConstants.textureData[dataIndex+2];

	float4 worldPos = float4(aWorldSpacePos, 1.0);
	float w = dot(v2, worldPos);
	if (w <= 0.0)
		return vec3(0);

    float4 v0 = SpotlightTextureConstants.textureData[dataIndex];
    float4 v1 = SpotlightTextureConstants.textureData[dataIndex+1];
    float4 v3 = SpotlightTextureConstants.textureData[dataIndex+3];

	float2 coord = float2(dot(v0, worldPos), dot(v1, worldPos));
	coord /= w;
	
	float3 uv = float3(coord * float2(0.5,-0.5) + vec2(0.5), v3.x);
	return 0;//MR_SampleLod0(SpotlightTexture, uv).xyz;
}

float GetAngularTextureAttenuation(Gfx_Light aLight, float3 aToLightVec)
{
	float dist = length(aToLightVec);
	if (dist < 0.001)
		return 1.0;

	float3 toLight = aToLightVec / dist;

	float u = acos(dot(toLight, aLight.direction)) / MR_PI;

	float2 uv = float2(u, aLight.angularTextureIndex);
	return 0;// MR_SampleLod0(AngularLightTexture, uv).x;
}

MR_ByteBuffer Gfx_TileLightData : MR_TileLightData;

@ifdef GFX_DEFERRED_COMPUTE
global uint Gfx_GetTileData_IndexOverride;
@endif // GFX_DEFERRED_COMPUTE

void Gfx_GetTileData(out uint baseIndex, out uint numLights, out float2 minMaxDist, float2 framebufferPixelCoord)
{
@ifdef GFX_DEFERRED_COMPUTE
	uint index = Gfx_GetTileData_IndexOverride;
@else
	float2 viewportPixelCoord = framebufferPixelCoord - Gfx_LightingEnvironment.ViewportOffset;

	float x = floor(viewportPixelCoord.x / 64);
	float y = floor(viewportPixelCoord.y / 64);

	uint index = (x + y * Gfx_LightingEnvironment.TileLightsNumXTiles);
@endif // GFX_DEFERRED_COMPUTE

@ifdef USE_MAKE_UNIFORM
	uint uindex = MR_MakeUniform(index);
@else
	uint uindex = index;
@endif // USE_MAKE_UNIFORM

	uint4 data = Gfx_TileLightData.Load4(uindex * 16);
	baseIndex = data.x;
	numLights = data.y;
	minMaxDist = asfloat(data.zw);
}

struct Gfx_DirectLight
{
	float3 diffuse;
	float3 spec;
};

Gfx_DirectLight Gfx_DiffuseSpec()
{
	Gfx_DirectLight ret;
	ret.diffuse = vec3(0);
	ret.spec = vec3(0);
	return ret;
}

Gfx_DirectLight Gfx_DiffuseSpec(float3 aDiffuse, float3 aSpec)
{
	Gfx_DirectLight ret;
	ret.diffuse = aDiffuse;
	ret.spec = aSpec;
	return ret;
}

struct Gfx_AttenuatedEmission
{
	float3 diffuseToLight;
	float3 diffuseEmission;

	float3 specToLight;
	float3 specEmission;
};

bool Gfx_LightHitsRooms(Gfx_Light aLight, uint aRoomBits)
{
@ifdef GFX_NO_INDOOR
	return true;
@else
	return (aLight.roomBits & aRoomBits) != 0;
@endif // GFX_NO_INDOOR
}

float Gfx_DistanceFade(Gfx_Light aLight, float aDistance)
{
	float attenuation = smoothstep(aLight.minNearAttenuationFadeDist, aLight.maxNearAttenuationFadeDist, aDistance);
	attenuation *= 1.0 - smoothstep(aLight.minAttenuationFadeDist, aLight.maxAttenuationFadeDist, aDistance);
	return attenuation;
}

float Gfx_AngleFade(Gfx_Light aLight, float3 aToLightVec)
{
	// angle fade
	float3 L = normalize(aToLightVec);
	float cosAlpha = saturate(dot(L, -aLight.direction));
	float angleAttenuation = saturate(mad(aLight.angleFadeMul, cosAlpha, aLight.angleFadeAdd));
	return angleAttenuation;
}

float3 Gfx_GetAttenuatedEmission_Internal1(Gfx_Light aLight, float3 aToLightVec, float3 aWorldSpacePos)
{
	float dist = max(length(aToLightVec) - aLight.lightSourceRadius, 0.0001);
	float denom = 0.03 + dist;
	float attenuation = 1 / (denom*denom);
	attenuation = attenuation - aLight.attenuationBias;
	attenuation = saturate(attenuation);
	attenuation *= Gfx_DistanceFade(aLight, dist);

	float3 emission = aLight.emission;

@ifndef GFX_DEBUG_NO_PROJECTED_TEXTURES
	if (aLight.projectedTextureDataIndex < 255)
		emission *= GetProjectedTextureEmission(aLight, aWorldSpacePos);
	else
@endif // GFX_DEBUG_NO_PROJECTED_TEXTURES
		attenuation *= Gfx_AngleFade(aLight, aToLightVec);

@ifndef GFX_DEBUG_NO_ANGULAR_TEXTURES
	if (aLight.angularTextureIndex < 255)
		attenuation *= GetAngularTextureAttenuation(aLight, aToLightVec);
@endif // GFX_DEBUG_NO_ANGULAR_TEXTURES

	return emission * attenuation;
}

bool Gfx_GetAttenuatedEmission_Internal2(out float3 aLightEmission, out float3 toLight, Gfx_Light aLight, float3 aWorldSpacePos)
{
	aLightEmission = vec3(0);
	
	float3 fromLight = aWorldSpacePos - aLight.position;
	float t2 = saturate(dot(fromLight, aLight.direction2)*aLight.direction2RcpLengthSquared);
	float t3 = saturate(dot(fromLight, aLight.direction3)*aLight.direction3RcpLengthSquared);
	float3 closest = aLight.position + t2*aLight.direction2 + t3*aLight.direction3;

	toLight = closest - aWorldSpacePos;
	if (dot(toLight, toLight) >= aLight.cutoffRange2)
		return false;

	float3 lightEmission = Gfx_GetAttenuatedEmission_Internal1(aLight, toLight, aWorldSpacePos);
	if (dot(lightEmission, vec3(1.0)) > 0.0)
	{
		aLightEmission = lightEmission;
		return true;
	}
	
	return false;
}

float2 Gfx_ProjectLLS(float3 aLightSpaceVector, Gfx_Light aLight)
{
	return float2(dot(aLightSpaceVector, aLight.direction2)*aLight.direction2RcpLengthSquared,
				  dot(aLightSpaceVector, aLight.direction3)*aLight.direction3RcpLengthSquared);
}

float3 Gfx_LLSToWS(float2 aLLSVector, Gfx_Light aLight)
{
	return aLight.position + aLLSVector.x*aLight.direction2 + aLLSVector.y*aLight.direction3;
}

bool Gfx_GetAttenuatedEmissionIdeal(out Gfx_AttenuatedEmission aEmissionOut, Gfx_Light aLight, float3 aWorldSpacePos, float3 anIdealVector)
{
	float3 fromLight = aWorldSpacePos - aLight.position;
	
	// closest point in local light space (2d-plane space for area lights, 1d-line space for tubes)
	float2 startLLS = Gfx_ProjectLLS(fromLight, aLight);
	float2 idealVectorLLS = Gfx_ProjectLLS(anIdealVector, aLight);

	// closest distance to plane/line of light
	float3 closestWS = Gfx_LLSToWS(startLLS, aLight);
	float closestDistance = length(closestWS - aWorldSpacePos);
	
	// the < test is for getting rid of QNAN pixels in deferred when an area light intersets geometry
	float closestDistance2 = dot(closestDistance, closestDistance);
	if ((closestDistance2 >= aLight.cutoffRange2) || (closestDistance2 < 0.000001))
		return false;
		
	float2 idealLLS = Gfx_ProjectLLS(anIdealVector, aLight);
	float2 offsetLLS = idealLLS*closestDistance/sqrt(clamp(1-dot(idealLLS,idealLLS), 0.01, 1));

	float safeAngle = 0.01;	

	float t = 1;
	if (offsetLLS.x > safeAngle)
		t = min((1-startLLS.x)/offsetLLS.x, t);
	else if (offsetLLS.x < -safeAngle)
		t = min(-startLLS.x/offsetLLS.x, t);

	if (offsetLLS.y > safeAngle)
		t = min((1-startLLS.y)/offsetLLS.y, t);
	else if (offsetLLS.y < -safeAngle)
		t = min(-startLLS.y/offsetLLS.y, t);

	float2 spI = startLLS + saturate(t)*offsetLLS; 
	float2 st = saturate(spI); 
	float3 bestWS = Gfx_LLSToWS(st, aLight);

	float3 closestToLight = Gfx_LLSToWS(saturate(startLLS), aLight) - aWorldSpacePos;

	float3 specToLight = bestWS - aWorldSpacePos;

@ifndef GFX_NO_SPEC_BENDING
	// bend spec direction to fake light source area 
	float specDist = length(specToLight);
	float3 idealScaled = anIdealVector*specDist;
	float3 delta = idealScaled - specToLight;
	float bendFactor = saturate(length(delta)/aLight.lightSourceRadius);
	float3 deltaN = normalize(delta);
	float3 bentSpecToLight = specToLight + bendFactor*deltaN*aLight.lightSourceRadius;
	specToLight = bentSpecToLight;
@endif // GFX_NO_SPEC_BENDING

	if (dot(closestToLight, closestToLight) >= aLight.cutoffRange2)
		return false;

	float3 emission = Gfx_GetAttenuatedEmission_Internal1(aLight, closestToLight, aWorldSpacePos);

	if (dot(emission, vec3(1.0)) > 0.0)
	{
		aEmissionOut.diffuseToLight = closestToLight;
		aEmissionOut.diffuseEmission = emission;
		aEmissionOut.specToLight = specToLight;
		aEmissionOut.specEmission = emission * aLight.specScale;
		return true;
	}

	return false;
}

bool Gfx_GetAttenuatedEmission(out Gfx_AttenuatedEmission aEmissionOut, Gfx_Light aLight, float3 aWorldSpacePos)
{
	if (!Gfx_GetAttenuatedEmission_Internal2(aEmissionOut.diffuseEmission, aEmissionOut.diffuseToLight, aLight, aWorldSpacePos))
		return false;

	aEmissionOut.specEmission = aEmissionOut.diffuseEmission * aLight.specScale;
	aEmissionOut.specToLight = aEmissionOut.diffuseToLight;

	return true;
}

bool Gfx_GetAttenuatedEmission(out Gfx_AttenuatedEmission aEmissionOut, Gfx_Light aLight, Gfx_LightingDataStruct aLightingData)
{
	if (!Gfx_GetAttenuatedEmission_Internal2(aEmissionOut.diffuseEmission, aEmissionOut.diffuseToLight, aLight, aLightingData.worldPosition))
		return false;

	aEmissionOut.specEmission = aEmissionOut.diffuseEmission * aLight.specScale;
	aEmissionOut.specToLight = aEmissionOut.diffuseToLight;

	return true;
}

bool Gfx_GetAttenuatedEmissionIdeal(out Gfx_AttenuatedEmission aEmissionOut, Gfx_Light aLight, Gfx_LightingDataStruct aLightingData, float3 aNormal)
{
@ifdef GFX_NO_IDEAL_SPEC
	if (!Gfx_GetAttenuatedEmission_Internal2(aEmissionOut.diffuseEmission, aEmissionOut.diffuseToLight, aLight, aLightingData.worldPosition))
		return false;

	aEmissionOut.specEmission = aEmissionOut.diffuseEmission * aLight.specScale;
	aEmissionOut.specToLight = aEmissionOut.diffuseToLight;

	return true;
@else
	float3 idealSpecDir = reflect(normalize(-aLightingData.toCamera), aNormal);
	return Gfx_GetAttenuatedEmissionIdeal(aEmissionOut, aLight, aLightingData.worldPosition, idealSpecDir);
@endif // GFX_NO_IDEAL_SPEC
}
