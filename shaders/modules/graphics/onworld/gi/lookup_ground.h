import "shaders/nodes/shader/functions/onward/OW_GetTerrainLowResHeight.h"

pixel
{
	MR_Sampler2DArray HeightMapCubeMap : MR_AdditionalTexture0 { filter = point, wrap = clamp };
	MR_Sampler2D SplatMapTexture : OW_SplatTexture { filter = linear, wrap = clamp };

	constants : MR_ConstantBuffer0
	{
		float const_planetMinRadius;
		float const_planetMaxRadius;
	}

	constants : MR_ConstantBuffer2
	{
		float4   const_planetsCenter;
		float4x4 const_faceTransform;
	}

	float3 GetWorldPosition( in float2 aUV, out float aHeight )
	{
		float2 pos2D = aUV * float2(2.0f,-2.0f) + float2(-1.0f,1.0f); // [0,1] => [-1,1]
		float3 pos3D = MR_Transform(const_faceTransform, float4(pos2D, 1.0f, 0.0f)).xyz;
		pos3D = normalize(pos3D);

		float3 uv = ConvertPosToHeightMapTextureArrayUV(pos3D);

		aHeight = MR_SampleLod0(HeightMapCubeMap, uv).x;
		return pos3D * lerp(const_planetMinRadius, const_planetMaxRadius, aHeight);
	}

	float3 GetWorldNormal( in float2 aUV, in float3 aWorldPosition )
	{
		float3 offset;
		offset.x = ddx(aUV).x;
		offset.y = ddy(aUV).y;
		offset.z = - offset.y;

		float4 heights;
		float3 v1 = GetWorldPosition( aUV - offset.xy, heights.x );
		float3 v2 = GetWorldPosition( aUV - offset.xz, heights.y );
		float3 v3 = GetWorldPosition( aUV + offset.xy, heights.z );
		float3 v4 = GetWorldPosition( aUV + offset.xz, heights.w );

		v1 = cross( (v2-aWorldPosition), (v1-aWorldPosition) );
		v2 = cross( (v4-aWorldPosition), (v3-aWorldPosition) );

		return -normalize( v1 + v2 );
	}
}