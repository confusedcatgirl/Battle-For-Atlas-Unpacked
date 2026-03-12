import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/environment.h"
import "shaders/modules/graphics/lighting_data.h"
import "shaders/nodes/Shader/Functions/Lighting/SS Reflection.h"
import "shaders/nodes/Shader/Functions/Fresnel-Schlick.h"

float3 WaterReflection(
    float3 defaultReflection,
    float3 reflectionTint <default = 1.0 1.0 1.0>,
    float3 depthTint,
    float depthScale <default = 0.6>,
    float refractionScale <default = 0.04>,
    float3 normal,
    float softDepthTestDistance <default = 0.5>,
    float3 Gfx_WorldSpacePos : Gfx_WorldSpacePos,
	float4 MR_FragCoord : MR_FragCoord,
	out float depthMask
)
{
	float4 clipPos0 = MR_Transform(MR_ViewConstants.worldToClip, Gfx_WorldSpacePos);
	float2 uv0 = MR_ClipToFramebufferUV(clipPos0.xy / clipPos0.w);

	float depth0 = MR_SampleLod0(Depthbuffer, uv0).x;

	float refractionDistance = max(0.0, depth0 - clipPos0.w);

	float normalScale = saturate(refractionDistance);
	normal = lerp(float3(0,1,0), normal, normalScale);

	Gfx_DirectionalLight sun = Gfx_GetShadowLight();

	float3 sunVec = sun.toLightDirection;
	float3 vertexToCamera = MR_GetPos(MR_ViewConstants.cameraToWorld).xyz - Gfx_WorldSpacePos;
	float3 toCamVec = normalize(vertexToCamera);
	
	float3 reflectionDirection = normalize(reflect(-toCamVec, normal));
	float3 refractionDirection = normalize(-toCamVec - normal);
	float3 refractionDirectionNoNormal = normalize(-toCamVec - float3(0,1,0));

	float3 left = normalize(cross(float3(0,1,0), toCamVec));
	float3 up = normalize(cross(toCamVec, left));
	float2 refraction2DNormal = float2(dot(left, refractionDirection), dot(up, refractionDirection));
	float2 refraction2DNoNormal = float2(dot(left, refractionDirectionNoNormal), dot(up, refractionDirectionNoNormal));
	float2 refraction2D = refraction2DNormal - refraction2DNoNormal;

	float4 reflectionCol0 = SSR_GetReflection(SSR_FramebufferEarly, Gfx_WorldSpacePos, MR_FragCoord, reflectionDirection, normal, Gfx_Environment.screenSpaceReflectionQuality, 1.0, 1.5);

	float3 reflectionCol = lerp(defaultReflection, reflectionCol0.xyz, reflectionCol0.a);
	reflectionCol *= reflectionTint;

	float3 refractionCol0 = MR_SampleLod0(SSR_FramebufferEarly, uv0 + refraction2D * normalScale * refractionScale).xyz;
	depthMask = exp(-max(0.0, depthScale * refractionDistance));
	float3 refractionCol = lerp(Gfx_GetEnvironmentGI(vec3(0.0)) * depthTint, refractionCol0, depthMask);

	float fresnel = FresnelSchlick(0.0, normal, toCamVec);
	float3 col = lerp(refractionCol, reflectionCol, fresnel);
	
	Gfx_LightingDataStruct ld = Gfx_GetLightingData(Gfx_WorldSpacePos, MR_FragCoord);
	col = Gfx_Fog(col, ld);

	float waterAlpha = saturate(refractionDistance / softDepthTestDistance);
	col = lerp(refractionCol0, col, waterAlpha);

	return col;
}
