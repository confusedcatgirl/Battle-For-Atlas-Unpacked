constants : MR_ConstantBuffer0
{
	float3 myWindVector;
	float myElapsedTime;
	int2 myCamMoveOffset;
	float myBaseHeight;
}

const uint SimSize = 128;
const float InvSimSize = 1. / SimSize;