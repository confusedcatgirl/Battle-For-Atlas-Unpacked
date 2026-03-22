uint4 MR_UnpackUint8VectorFromUint32(uint aDataChunk)
{
	uint4 returnValue;
	returnValue.x =  aDataChunk 		& 0xff;
	returnValue.y = (aDataChunk >> 8 ) 	& 0xff;
	returnValue.z = (aDataChunk >> 16) 	& 0xff;
	returnValue.w = (aDataChunk >> 24) 	& 0xff;
	return returnValue;
}

int4 MR_UnpackInt8VectorFromInt32(int aDataChunk)
{
	int4 returnValue;
	returnValue.x = ((aDataChunk << 24) 	>> 24);
	returnValue.y = ((aDataChunk << 16) 	>> 24);
	returnValue.z = ((aDataChunk << 8 ) 	>> 24);
	returnValue.w =  (aDataChunk    		>> 24);
	return returnValue;
}

int4 MR_UnpackInt8VectorFromUint32(uint aDataChunk)
{
	int signedDataChunk = asint(aDataChunk);
	return MR_UnpackInt8VectorFromInt32(signedDataChunk);
}

float4 MR_UnpackUnorm8VectorFromUint32(uint aDataChunk)
{
	return MR_UnpackUint8VectorFromUint32(aDataChunk) / 255.0f;
}

float4 MR_UnpackSnorm8VectorFromInt32(int aDataChunk)
{
	return MR_UnpackInt8VectorFromInt32(aDataChunk) / 127.0f;
}