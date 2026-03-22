
struct Gfx_LightingDataStruct
{
	float3 worldPosition;
	float3 toCamera;
	float2 pixelPosition;
	float depth;
	int roomIndex;
	uint roomBits;
	bool isIndoor;
	bool isGround;
	bool isUnflatten;
	float SSAO;
	bool isMoving;
};
