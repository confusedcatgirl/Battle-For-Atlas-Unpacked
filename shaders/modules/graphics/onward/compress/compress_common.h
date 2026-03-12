uint ColorTo565( float3 color )
{
	uint3 rgb = round( color * float3(31.0f, 63.0f, 31.0f) );
	return (rgb.r << 11) | (rgb.g << 5) | rgb.b;
}

uint GetIndicesRGB( float3 block[16], float3 minColor, float3 maxColor )
{
	uint indices = 0;

	// For each input color, we need to select between one of the following output colors:
	//  0: maxColor
	//  1: (2/3)*maxColor + (1/3)*minColor
	//  2: (1/3)*maxColor + (2/3)*minColor
	//  3: minColor
	//
	// We essentially just project (block[i] - maxColor) onto (minColor - maxColor), but we pull out
	//  a few constant terms.
	float3 diag = minColor - maxColor;
	float stepInc = 3.0f / dot(diag, diag); // Scale up by 3, because our indices are between 0 and 3
	diag *= stepInc;
	float c = stepInc * ( dot(maxColor, maxColor) - dot(maxColor, minColor) ) ;

	for(int i = 15; i >= 0; --i)
	{
		// Compute the index for this block element
		uint index = round( dot(block[i], diag) + c );

		// Now we need to convert our index into the somewhat unintuivive BC1 indexing scheme:
		//  0: maxColor
		//  1: minColor
		//  2: (2/3)*maxColor + (1/3)*minColor
		//  3: (1/3)*maxColor + (2/3)*minColor
		//
		// The mapping is:
		//  0 -> 0
		//  1 -> 2
		//  2 -> 3
		//  3 -> 1
		//
		// We can perform this mapping using bitwise operations, which is faster
		//  than predication or branching as long as it doesn't increase our register
		//  count too much. The mapping in binary looks like:
		//  00 -> 00
		//  01 -> 10
		//  10 -> 11
		//  11 -> 01
		//
		// Splitting it up by bit, the output looks like:
		//  bit1_out = bit0_in XOR bit1_in
		//  bit0_out = bit1_in
		uint bit0_in = index & 1;
		uint bit1_in = index >> 1;
		indices |= ((bit0_in^bit1_in) << 1) | bit1_in;

		if(i != 0)
		{
			indices <<= 2;
		}
	}

	return indices;
}

void GetMinMaxRGB( float3 colorBlock[16], out float3 minColor, out float3 maxColor )
{
	float3 midColor = float3(0,0,0);

	for(int i = 0; i < 16; ++i)
	{
		midColor += colorBlock[i];
	}
	midColor *= 1.0/16;

	float3 offColor = float3(0,0,0);
	float offDist = -1;

	for(int i = 0; i < 16; ++i)
	{
		float dist = dot(colorBlock[i]-midColor, colorBlock[i]-midColor);
		if(dist > offDist)
		{
			offColor = colorBlock[i];
			offDist = dist;
		}
	}

	float3 offColor2 = float3(0,0,0);
	float offDist2 = -1;

	for(int i = 0; i < 16; ++i)
	{
		float dist = dot(colorBlock[i]-offColor, colorBlock[i]-offColor);
		if(dist > offDist2)
		{
			offColor2 = colorBlock[i];
			offDist2 = dist;
		}
	}
	minColor = offColor;
	maxColor = offColor2;
/*/
	minColor = colorBlock[0];
	maxColor = colorBlock[0];

	for(int i = 1; i < 16; ++i)
	{
		minColor = min(minColor, colorBlock[i]);
		maxColor = max(maxColor, colorBlock[i]);
	}/**/
}

uint2 CompressBC1Block( float3 block[16], float colorScale)
{
	float3 minColor, maxColor;
	GetMinMaxRGB( block, minColor, maxColor );

	// Pack our colors into uints
	uint minColor565 = ColorTo565( colorScale * minColor );
	uint maxColor565 = ColorTo565( colorScale * maxColor );

	if(minColor565 > maxColor565)
	{
		uint temp = minColor565;
		minColor565 = maxColor565;
		maxColor565 = temp;
		float3 temp2 = minColor;
		minColor = maxColor;
		maxColor = temp2;
	}
	uint indices = 0;
	if(minColor565 < maxColor565)
	{
		indices = GetIndicesRGB( block, minColor, maxColor );
	}

	return uint2( (minColor565 << 16) | maxColor565, indices );
}

void GetMinMaxAlpha( float block[16], out float minC, out float maxC )
{
	minC = block[0];
	maxC = block[0];

	for(int i = 1; i < 16; ++i)
	{
		minC = min(minC, block[i]);
		maxC = max(maxC, block[i]);
	}
}

void GetIndicesAlpha( float block[16], float minA, float maxA, inout uint2 packedValue)
{
	float d = minA-maxA;
	float stepInc = 7.0f / d;

	// Both packedValue.x and packedValue.y contain index values, so we need two loops

	uint index = 0;
	uint shift = 16;
	for(int i = 0; i < 6; ++i)
	{
		// For each input alpha value, we need to select between one of eight output values
		//  0: maxA
		//  1: (6/7)*maxA + (1/7)*minA
		//  ...
		//  6: (1/7)*maxA + (6/3)*minA
		//  7: minA
		index = round( stepInc * (block[i]-maxA) );

		// Now we need to convert our index into the BC indexing scheme:
		//  0: maxA
		//  1: minA
		//  2: (6/7)*maxA + (1/7)*minA
		//  ...
		//  7: (1/7)*maxA + (6/3)*minA
		index += (index > 0) - (7 * (index == 7));

		packedValue.x |= (index << shift);
		shift += 3;
	}

	// The 6th index straddles the two uints
	packedValue.y |= (index >> 1);

	shift = 2;
	for(int i = 6; i < 16; ++i)
	{
		index = round( (block[i]-maxA) * stepInc );
		index += (index > 0) - (7 * (index == 7) );

		packedValue.y |= (index << shift);
		shift += 3;
	}
}

uint2 CompressBC4Block( float block[16])
{
	float minA, maxA;
	GetMinMaxAlpha( block, minA, maxA );

	uint minAPacked = round(minA * 255.0f);
	uint maxAPacked = round(maxA * 255.0f);
	uint2 outA = uint2((minAPacked << 8) | maxAPacked,0);

	if(minAPacked < maxAPacked)
	{
		GetIndicesAlpha(block, minA, maxA, outA);
	}
	return outA;
}
