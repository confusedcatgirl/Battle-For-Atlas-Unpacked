float OW_Curve_CheapStep(float aX)
{
	aX = 1.0 - aX * aX;
	aX = 1.0 - aX * aX;

	return aX;
}

float OW_Curve_CheapStepInv(float aX)
{
	aX = 1.0 - aX * aX;

	return aX * aX;
}

float OW_Curve_SymmetricalBellShaped(in float aX, in float aPeak, in float aHalfWidth)
{
	aX = abs(aX - aPeak);
	aX = aX / aHalfWidth;
	return OW_Curve_CheapStepInv( saturate(aX) );
}

float OW_Curve_SymmetricalBellShaped(in float aX, in float aHalfWidth)
{
	return OW_Curve_SymmetricalBellShaped( aX, aHalfWidth, aHalfWidth );
}

// Bell curve, but different slope on each side.  Basically, if X is smaller than the peak, we divide X by 'peak', otherwise by 'width - peak'... then smooth it

float OW_Curve_AsymmetricalBellShaped(in float aX, in float aPeak, in float aWidth)
{
	aX = aX - aPeak;
	aX = aX / lerp(-aPeak, aWidth - aPeak, step(0, aX));
	return OW_Curve_CheapStepInv( saturate(aX) );
}
