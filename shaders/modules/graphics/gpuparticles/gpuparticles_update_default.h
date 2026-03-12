import "shaders/modules/graphics/gpuparticles/gpuparticles_common.h"
import "shaders/MRender/MR_ViewConstants.h"
import "shaders/modules/graphics/onworld/planet_flattening.h"
import "shaders/modules/graphics/onward/gi/ground_fog.h"
import "shaders/nodes/Shader/Functions/Onward/OW_GetTerrainLowResRadius.h"

void UpdateParticles(in int aParticleIndex,  in MR_Sampler2D<float> aDepthBuffer, inout SParticleData aParticleData, out SOutputVertices someOutputVertices)
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
	float3 wind = localwind + RandomPosNoise(pos, startTime*0.5) * float3(1, 0.6, 1) * myTurbulence * (1 + length(localwind)) * 0.5;
	float dragPart = pow(1-mass, myInvElapsedTime);
	velocity = velocity * (1-dragPart) + wind * dragPart;

	pos += velocity * myElapsedTime;

	// debugColor = float4(0.0f, 10.0f, 0.0f, 1.0f);

	float3 modCameraPos = float3(MR_ViewConstants.cameraToWorld[0][3], MR_ViewConstants.cameraToWorld[1][3], MR_ViewConstants.cameraToWorld[2][3]);
	modCameraPos.x += MR_ViewConstants.cameraToWorld[0][2] * SnowFieldRange * 0.8;
	modCameraPos.y += MR_ViewConstants.cameraToWorld[1][2] * SnowFieldRangeY * 0.8;
	modCameraPos.z += MR_ViewConstants.cameraToWorld[2][2] * SnowFieldRange * 0.8;

	bool teleported = false;
	if(pos.x < modCameraPos.x - SnowFieldRange)
	{
		pos.x += SnowFieldRange*2;
		pos.yz = GetRandomPos(modCameraPos, flags).yz;
		teleported = true;
	}
	if(pos.x > modCameraPos.x + SnowFieldRange)
	{
		pos.x -= SnowFieldRange*2;
		pos.yz = GetRandomPos(modCameraPos, flags).yz;
		teleported = true;
	}
	if(pos.y < modCameraPos.y - SnowFieldRangeY)
	{
		pos.y += SnowFieldRangeY*2;
		pos.xz = GetRandomPos(modCameraPos, flags).xz;
		teleported = true;
	}
	if(pos.y > modCameraPos.y + SnowFieldRangeY)
	{
		pos.y -= SnowFieldRangeY*2;
		pos.xz = GetRandomPos(modCameraPos, flags).xz;
		teleported = true;
	}
	if(pos.z < modCameraPos.z - SnowFieldRange)
	{
		pos.z += SnowFieldRange*2;
		pos.xy = GetRandomPos(modCameraPos, flags).xy;
		teleported = true;
	}
	if(pos.z > modCameraPos.z + SnowFieldRange)
	{
		pos.z -= SnowFieldRange*2;
		pos.xy = GetRandomPos(modCameraPos, flags).xy;
		teleported = true;
	}

	if(teleported)
	{
		terrainHeight = OW_GetTerrainLowResRadius( pos );

		flags &= ~32;

		if (terrainHeight > length(pos - myPlanetPos))
			flags |= 32;

		aParticleData.myFlags = flags;
	}

	aParticleData.myPos = pos;
	aParticleData.myVelocity = velocity;

	OW_SFT_ApplyAll(pos, OW_DEFAULT_CATEGORY);

	float3 camPos = MR_Transform(MR_ViewConstants.unjitteredWorldToCamera, pos).xyz;

	bool onScreen = false;
	if(camPos.z > 0.1)
	{
		float4 clipPos = MR_Transform(MR_ViewConstants.cameraToClip, camPos);
		float2 uv = MR_ClipToFramebufferUV(clipPos.xy / clipPos.w);
		onScreen = uv.x > 0 && uv.x < 1 && uv.y > 0 && uv.y < 1;

		// this is a safe depth value to avoid flickering on edges
		float depth = 100;

		if (onScreen)
			depth = MR_SampleLod0(aDepthBuffer, uv);

		if(camPos.z > depth+0.02)
		{
			onScreen = false;
			if(camPos.z < depth+1)
			{
				flags |= 32;
				aParticleData.myFlags = flags;
			}
		}
	}
	if((onScreen && terrainHeight-0.1 > pos.y && terrainHeight-0.3 < pos.y) || (!onScreen && terrainHeight > pos.y))
	{
		flags |= 32;
		aParticleData.myFlags = flags;
	}

	if(flags&32)
	{
		camPos.z = -100.0f;	//make invisible
	}

	const OW_FogVolume3DCoords fogCoords = OW_GroundFog_GetCoordsFromPos( pos );
	const float4 fog = OW_GroundFog_GetFromCoords( fogCoords );
	const float3 lighting = (MR_SampleLod0( PlanetLightVolume, float3( fogCoords.my2D.myUV, fogCoords.myW ) ).rgb * fog.w ) + fog.xyz;

	float alpha = min(2, min(age, myLifeTime-age))*0.5;

	float camDist = camPos.z;
	alpha = max(0 ,min(alpha, camDist*1.5-0.3));
	if(size*MR_ViewConstants.viewportSize.x < camDist*2)
	{
		float factor = camDist*2 / (size*MR_ViewConstants.viewportSize.x);
		size *= factor;
		alpha /= factor*factor;
	}

	alpha *= fog.w; // CDP: Since we don't sort, I think it's better to do this

	const uint uvOffsetX = (flags&3)*64; // This controls the uv.u, selecting 1 in 4 columns
	const uint uvOffsetY = ((flags&16)/16)*128*256; // This controls the uv.v, selecting 1 in 2 rows
	const uint uvOffsetZ = ((flags&4)/4)*255*256*256; // These controls the direction of the uvs
	const uint uvOffsetW = ((flags&8)/8)*255*256*256*256;
	const uint textureIdx = uvOffsetX + uvOffsetY + uvOffsetZ + uvOffsetW;

	bool streaky = flags & 16;
	float3 camDir = float3(0.0f, 1.0f, 0.0f);
	if(streaky)
	{
		float3 camVelocity = MR_TransformNormal(MR_ViewConstants.unjitteredWorldToCamera, velocity);
		camDir = camVelocity * (myElapsedTime*0.0+0.006) /** min(camDist*0.4, 1)*/;
		float camDirLength = length(camDir)+0.0001;
		camDir = camDir/camDirLength*(1+camDirLength/size);
	}
	else
	{
		camDir = float3(snoise(float4(pos, myCurrentTime+300+age+(flags&127)), 2, 0.25, 1, 0.5, 1), snoise(float4(pos, myCurrentTime+400+age+(flags&127)), 2, 0.25, 1, 0.5, 1), 0.5);
		camDir = normalize(camDir);
	}

	someOutputVertices.myCameraPos = camPos;
	someOutputVertices.myTextureIdx = textureIdx;
	someOutputVertices.myLighting = lighting;
	someOutputVertices.myAlpha = alpha;
	someOutputVertices.myCameraDir = camDir;
	someOutputVertices.mySize = size;
}