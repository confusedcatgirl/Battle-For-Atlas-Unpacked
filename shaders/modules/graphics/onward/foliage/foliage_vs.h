@define USE_BRANCHLESS

@include "shaders/modules/graphics/onward/foliage/foliage_base.h"

constants : MR_ConstantBuffer0
{
    float constResistance;
    float constMeshLength;
}
vertex
{
	import "shaders/modules/graphics/onworld/planet_flattening.h"

	import "shaders/modules/graphics/onward/foliage/foliage_wind.h"
	import "shaders/modules/graphics/onward/foliage/foliage_force.h"

	import "shaders/nodes/Shader/Functions/Onward/OW_GetTerrainLowResRadius.h"

	in
	{
		float3 localPos;

@ifndef DEPTH
		float4 col;
@ifndef GRASS
		float2 uvs;
		float3 nrm;
		float3 tan;
@endif // GRASS
@else
@ifndef GRASS
		float2 uvs;
@endif // GRASS
@endif // DEPTH

		stream 1 steprate=1
		{
			float4 instancePos;
			float4 instanceCol;
		}
	}

	float3 TransfromVertex( in float3 aLocalPos, in float3 anInstancePos, in float4 someAttribs2, in float3 tangent, out float3 aNormalOut, out float3 aTangentOut, out float3 aBinormalOut, out TransfromOutput anOutput )
	{
		const float3 upVector = normalize( anInstancePos );
		{
			aNormalOut = upVector;
		}

		float3x3 transform = OW_Foliage_CreateTransform( upVector, float4( upVector, 0.0f ) );

		float4 vertexPos = float4( aLocalPos, constMeshLength ) * someAttribs2.wwww;
		anOutput.params.xy = saturate( vertexPos.yy / vertexPos.ww );
		float3 pos = anInstancePos + MR_Transform( transform, vertexPos.xyz );

		float3 waves = OW_Foliage_GetWindWaves( anInstancePos, vertexPos.xyz );
		@ifndef USE_FORCE_FIELD
			anOutput.forces = OW_Foliage_GetWindDirection( waves, transform );
		@else
			anOutput.forces = OW_GetForceField( pos );
		@endif // USE_FORCE_FIELD

		anOutput.forces = OW_Foliage_GetWindForce( waves, anOutput.forces );
		anOutput.forces *= constResistance * anOutput.params.y;

@ifdef HAS_NORMAL_TEXTURE
		aTangentOut = MR_Transform( transform, tangent );
		aBinormalOut = cross( tangent, upVector );
@else
		aTangentOut = float3(1, 0, 0);
		aBinormalOut = float3(0, 1, 0); 
@endif // HAS_NORMAL_TEXTURE

		const float3 vertexUp = normalize( pos );
		float lowResRadius = OW_GetTerrainLowResRadius( vertexUp );
		const float3 onGround = vertexUp * lowResRadius;
		float3 direction = normalize( vertexUp + anOutput.forces );
		pos = onGround + direction * vertexPos.y;

		OW_SFT_ApplyFlatteningLocal(pos, OW_DEFAULT_FOLIAGE);
		pos += OW_ViewConstants.flatteningParams.myPlanetCenter;

		anOutput.params.z = saturate( length( anOutput.forces * 2.0f ) );

		return pos;
	}
}