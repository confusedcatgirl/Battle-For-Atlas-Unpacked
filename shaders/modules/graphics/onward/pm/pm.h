
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct OGfx_Particulate
{
	float3 myPosition;
	float  myAge;
	float3 myVelocity;
	uint   myTag;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool OGfx_PM_IsDead( in OGfx_Particulate aParticle )
{
	return (aParticle.myTag & 1) == 1;
}

void OGfx_PM_SetDead( inout OGfx_Particulate aParticle )
{
	aParticle.myTag |= 1;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

uint OGfx_PM_GetRandomIndex( in OGfx_Particulate aParticle )
{
	return (aParticle.myTag >> 16);
}

void OGfx_PM_SetRandomIndex( inout OGfx_Particulate aParticle, in uint aIndex )
{
	aParticle.myTag |= (aIndex << 16);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

uint OGfx_PM_GetLocalIndex( in OGfx_Particulate aParticle )
{
	return (aParticle.myTag & 0xFFFF) >> 1;
}

void OGfx_PM_SetLocalIndex( inout OGfx_Particulate aParticle, in uint aIndex )
{
	aParticle.myTag |= ((aIndex << 1) & 0xFFFF);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MR_Buffer< float4 > OGfx_GP_RandomNumbers : MR_Buffer7;
const uint OGfx_GP_RandomNumber_Count = 65536;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
