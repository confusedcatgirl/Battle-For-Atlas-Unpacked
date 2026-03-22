import "shaders/modules/graphics/environment.h"
import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/shadows.h"

float2 ParallaxMapping(float2 aUV, MR_Sampler2D aHeight, float aScale, float3x3 Gfx_TangentToWorld : Gfx_TangentToWorld0,	float3 Gfx_WorldSpacePos : Gfx_WorldSpacePos0)
{
	float2 retUV = aUV;
	
@ifdef PARALLAX_MAPPING
	float3 vertexToCamera = MR_GetPos(MR_ViewConstants.cameraToWorld).xyz - Gfx_WorldSpacePos;
	float toCameraDistance = length(vertexToCamera);
	if (toCameraDistance > Gfx_Environment.parallaxDistance)
		return retUV;
	
	vertexToCamera = normalize(vertexToCamera);
	float3x3 worldToTangent = transpose(Gfx_TangentToWorld);
	
	float3 cameraToVertexTan = MR_Transform(worldToTangent, -vertexToCamera);
	cameraToVertexTan.x = -cameraToVertexTan.x;
	
	Gfx_DirectionalLight shadowLight = Gfx_GetShadowLight();
	float3 toLightTan = MR_Transform(worldToTangent, shadowLight.toLightDirection);
	toLightTan.x = -toLightTan.x;
	
	float parallaxScale = Gfx_Environment.parallaxScale * aScale;
	
	float2 dx = ddx(aUV);
	float2 dy = ddy(aUV);

@ifdef PARALLAX_OCCLUSION_MAPPING
	float height = 1.0;
	float2 uv = aUV;
	
	float numSamples = lerp(Gfx_Environment.parallaxPOMMaxSamples, Gfx_Environment.parallaxPOMMinSamples, saturate(dot(vertexToCamera, MR_GetZAxis(Gfx_TangentToWorld))));
	float heightStep = 1.0 / numSamples;
	float2 uvStep = cameraToVertexTan.xy * parallaxScale * heightStep / cameraToVertexTan.z;
	
	float sampledHeight = MR_SampleGrad(aHeight, uv, dx, dy).r;
	float lastHeight = height;
	float lastSampledHeight = sampledHeight;
	while (sampledHeight <= height)
	{
		lastHeight = height;
		lastSampledHeight = sampledHeight;
		
		height -= heightStep;
		uv += uvStep;
		sampledHeight = MR_SampleGrad(aHeight, uv, dx, dy).r;
	}
	
	float lastDiff = lastHeight - lastSampledHeight;
	float currDiff = height - sampledHeight;
	
	float denom = lastDiff - currDiff;
	float offset = 0.0;
	if (denom != 0.0)
		offset = (height * lastDiff - lastHeight * currDiff) / denom;
		
	float2 intersection = aUV + cameraToVertexTan.xy * parallaxScale * (1.0 - offset) / cameraToVertexTan.z;
	retUV = intersection.xy;

	@ifndef GFX_DEBUG_NO_SHADOWS
		@ifdef PARALLAX_SHADOWS
			float2 uvStepShadow = -toLightTan.xy * parallaxScale * Gfx_Environment.parallaxShadowScale / toLightTan.z;
			float h0 = MR_SampleGrad(aHeight, intersection.xy, dx, dy).r;
			float h = MR_SampleGrad(aHeight, intersection.xy + uvStepShadow, dx, dy).r * 0.7;
			h = max(h, MR_SampleGrad(aHeight, intersection.xy + 0.66 * uvStepShadow, dx, dy).r * 0.85);
			h = max(h, MR_SampleGrad(aHeight, intersection.xy + 0.33 * uvStepShadow, dx, dy).r);
			
			ourParallaxShadowFactor = 1.0 - saturate((h - h0) * Gfx_Environment.parallaxShadowStrength);
		@endif
	@endif
@endif

@ifdef PARALLAX_SIMPLE
	float2 uvStep = cameraToVertexTan.xy * parallaxScale / cameraToVertexTan.z;
	float sampledHeight = MR_SampleGrad(aHeight, aUV, dx, dy).r;
	retUV = aUV + (1.0 - sampledHeight) * uvStep;

	//float2 uvStep = cameraToVertexTan.xy * parallaxScale / cameraToVertexTan.z;
	//float sampledHeight = MR_SampleGrad(aHeight, aUV, dx, dy).r;
	//sampledHeight += MR_SampleGrad(aHeight, (1.0 - sampledHeight) * uvStep, dx, dy).r;
	//retUV = aUV + (1.0 - sampledHeight / 2.0) * uvStep;
@endif

	float lerpValue = saturate((Gfx_Environment.parallaxDistance - toCameraDistance));
@ifndef GFX_DEBUG_NO_SHADOWS
	@ifdef PARALLAX_SHADOWS
		ourParallaxShadowFactor = lerp(1.0, ourParallaxShadowFactor, lerpValue);
	@endif
@endif

	retUV = lerp(aUV, retUV, lerpValue);
@endif

	return retUV;
}