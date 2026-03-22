
MR_ByteBuffer OGfx_PM_EmitterBuffer : MR_Buffer15;

struct OGfx_PM_EmitterProps
{
	float4 myPositionRadius;

	float myMaxSize;
	float myMaxAge;
	float myMaxMass;

	uint myConfig;
};

OGfx_PM_EmitterProps OGfx_PM_UnpackEmitterProps(uint4 someData[2])
{
	OGfx_PM_EmitterProps props;

	props.myPositionRadius = asfloat(someData[0]);

	const float3 floats = asfloat(someData[1].xyz);

	props.myMaxSize = floats.x;
	props.myMaxAge  = floats.y;
	props.myMaxMass = floats.z;

	props.myConfig = asuint(someData[1].w);

	return props;
}

OGfx_PM_EmitterProps OGfx_PM_GetEmitterProps(uint aIndex)
{
	uint dataIndex = aIndex * (2 * 16);

	uint4 data[ 2 ];
	data[ 0 ] = OGfx_PM_EmitterBuffer.Load4( dataIndex + (0 * 16) );
	data[ 1 ] = OGfx_PM_EmitterBuffer.Load4( dataIndex + (1 * 16) );

	return OGfx_PM_UnpackEmitterProps( data );
}
