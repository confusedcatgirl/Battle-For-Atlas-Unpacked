import "shaders/modules/graphics/gpuparticles/gpuparticles_common.h"
import "shaders/modules/graphics/gpuparticles/simplexnoise4d.h"
import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/onworld/planet_flattening.h"
import "shaders/modules/graphics/onward/gi/ground_fog.h"
import "shaders/nodes/Shader/Functions/Onward/OW_GetTerrainLowResRadius.h"

void UpdateParticles(in int aParticleIndex, in MR_StructuredBuffer< SParticleSpawnVolume > someSpawnVolumes, 
					in MR_Sampler2D<float> aDepthBuffer, inout SParticleData aParticleData, out SOutputVertices someOutputVertices)
{
	float3 pos = aParticleData.myPos;
	float3 velocity = aParticleData.myVelocity;
	float startTime = aParticleData.myStartTime;
	float age = myCurrentTime - startTime;
	float size = aParticleData.mySize;
	float mass = aParticleData.myMass;
	uint flags = aParticleData.myFlags;

	const float3 localwind = myWindVector;
	float terrainHeight = OW_GetTerrainLowResRadius( pos );

	float distanceToCamera = 0.0f;

	velocity.y -= 10 * myElapsedTime;	//gravity
	float3 wind = localwind + RandomPosNoise(pos * 0.1f, startTime * 0.5f) * 5.0f * myTurbulence * (1 + length(localwind)) * 0.5;
	float dragPart = pow(1-mass, myInvElapsedTime);
	velocity = velocity * (1-dragPart) + wind * dragPart;

	pos += velocity * myElapsedTime;
	// debugColor = float4(10.0f, 0.0f, 0.0f, 1.0f);

	const uint spawnVolumeIndex = GetSpawnVolumeIndex( aParticleIndex, mySpawnVolsCount);
	distanceToCamera = someSpawnVolumes[spawnVolumeIndex].myDistanceToCamera;
	if (IsParticleCulled(distanceToCamera))
	{
		flags |= 32;
	}

	if(terrainHeight > length(pos - myPlanetPos))
		flags |= 32;

	aParticleData.myPos = pos;
	aParticleData.myVelocity = velocity;

	OW_SFT_ApplyAll(pos, OW_DEFAULT_CATEGORY);

	float3 camPos = MR_Transform(MR_ViewConstants.unjitteredWorldToCamera, pos).xyz;

	if(flags&32)
	{
		camPos.z = -100.0f;	//make invisible
	}

	const OW_FogVolume3DCoords fogCoords = OW_GroundFog_GetCoordsFromPos( pos );
	const float4 fog = OW_GroundFog_GetFromCoords( fogCoords );
	const float3 lighting = ((MR_SampleLod0( PlanetLightVolume, float3( fogCoords.my2D.myUV, fogCoords.myW ) ).rgb * fog.w ) + fog.xyz) * float3(0.1f, 0.23f, 0.84f);

	float alpha = min(2, min(age, myLifeTime-age))*0.5;

	alpha *= fog.w; // CDP: Since we don't sort, I think it's better to do this

	const uint uvOffsetX = (flags&3)*64; // This controls the uv.u, selecting 1 in 4 columns
	const uint uvOffsetY = ((flags&16)/16)*128*256; // This controls the uv.v, selecting 1 in 2 rows
	const uint uvOffsetZ = ((flags&4)/4)*255*256*256; // These controls the direction of the uvs
	const uint uvOffsetW = ((flags&8)/8)*255*256*256*256;
	const uint textureIdx = uvOffsetX + uvOffsetY + uvOffsetZ + uvOffsetW;

	float3 camDir = float3(snoise(float4(pos, myCurrentTime+300+age+(flags&127)), 2, 0.01, 1, 0.5, 1), 
					snoise(float4(pos, myCurrentTime+400+age+(flags&127)), 2, 0.01, 1, 0.5, 1), 
					0.5);
	camDir = normalize(camDir);

	someOutputVertices.myCameraPos = camPos;
	someOutputVertices.myTextureIdx = textureIdx;
	someOutputVertices.myLighting = lighting;
	someOutputVertices.myAlpha = alpha;
	someOutputVertices.myCameraDir = camDir;
	someOutputVertices.mySize = size;
}