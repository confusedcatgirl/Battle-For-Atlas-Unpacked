
MR_ByteBuffer OGfx_PM_EmitterBuffer : MR_Buffer15;

struct OGfx_PM_EmitterProps
{
	float3 myOffset;
	uint myConfig;
	int4 mySegment;

	float myMaxSize;
	float myMaxAge;
	float myMaxMass;
};

OGfx_PM_EmitterProps OGfx_PM_UnpackEmitterProps(uint4 someData[3])
{
	OGfx_PM_EmitterProps props;

	props.myOffset = asfloat(someData[0].xyz);
	props.myConfig = asuint(someData[0].w);
	props.mySegment = asint(someData[1]);

	const float3 floats = asfloat(someData[2].xyz);
	props.myMaxSize = floats.x;
	props.myMaxAge  = floats.y;
	props.myMaxMass = floats.z;

	return props;
}

OGfx_PM_EmitterProps OGfx_PM_GetEmitterProps(uint aIndex)
{
	uint dataIndex = aIndex * (3 * 16);

	uint4 data[ 3 ];

	data[ 0 ] = OGfx_PM_EmitterBuffer.Load4( dataIndex + (0 * 16) );
	data[ 1 ] = OGfx_PM_EmitterBuffer.Load4( dataIndex + (1 * 16) );
	data[ 2 ] = OGfx_PM_EmitterBuffer.Load4( dataIndex + (2 * 16) );

	return OGfx_PM_UnpackEmitterProps( data );
}
