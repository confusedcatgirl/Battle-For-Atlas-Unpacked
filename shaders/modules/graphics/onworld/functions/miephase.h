
float OW_MiePhase(float aCos, float aCos2, float aG, float aG2)
{
    return (3.0*(1.0-aG2))/(2.0*(2.0+aG2)) * ((1.0 + aCos2) / pow(1.0 + aG2 - 2.0 * aG * aCos, 1.5));
}

float OW_MiePhase(float aCos, float aCos2, float aG)
{
    return OW_MiePhase(aCos, aCos2, aG, aG * aG);
}