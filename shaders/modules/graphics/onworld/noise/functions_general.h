
float OW_Noise_Hash1To1(in float p)
{
    return frac(sin(p) * 43758.5453123f);
}

float OW_Noise_Hash2To1(in float2 p)
{
    float h = dot(p,float2(127.1f,311.7f));
    return frac(sin(h) * 43758.5453123f);
}

float3 OW_Noise_Hash1To3(float p)
{
    float3 h = float3(1275.231f, 4461.7f, 7182.423f) * p;	
    return frac(sin(h) * 43758.5453123f);
}

float OW_Noise_2D(in float2 x)
{
    float2 p = floor(x);
    float2 f = frac(x);
    f = f*f*(3.0f-2.0f*f);

    return lerp(lerp(OW_Noise_Hash2To1(p + float2(0.0f,0.0f)), 
                     OW_Noise_Hash2To1(p + float2(1.0f,0.0f)),f.x),
                lerp(OW_Noise_Hash2To1(p + float2(0.0f,1.0f)), 
                     OW_Noise_Hash2To1(p + float2(1.0f,1.0f)),f.x),f.y);
}

float OW_Noise_3D(in float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);
    f = f*f*(3.0f-2.0f*f);

    float n = p.x + p.y * 157.0f + 113.0f * p.z;
    return lerp(lerp(lerp( OW_Noise_Hash1To1(n+  0.0f), OW_Noise_Hash1To1(n+  1.0f),f.x),
                     lerp( OW_Noise_Hash1To1(n+157.0f), OW_Noise_Hash1To1(n+158.0f),f.x),f.y),
                lerp(lerp( OW_Noise_Hash1To1(n+113.0f), OW_Noise_Hash1To1(n+114.0f),f.x),
                     lerp( OW_Noise_Hash1To1(n+270.0f), OW_Noise_Hash1To1(n+271.0f),f.x),f.y),f.z);
}

const float3x3 fbmRot3 = float3x3( 0.00f,  1.60f,  1.20f, -1.60f,  0.72f, -0.96f, -1.20f, -0.96f,  1.28f );

// Fractional Brownian motion
float OW_Noise_FBM( in float3 p )
{
    float f  = 0.5000f * OW_Noise_3D( p ); p = MR_Transform(fbmRot3, p) * 1.1f;
          f += 0.2500f * OW_Noise_3D( p ); p = MR_Transform(fbmRot3, p) * 1.2f;
          f += 0.1666f * OW_Noise_3D( p ); p = MR_Transform(fbmRot3, p);
          f += 0.0834f * OW_Noise_3D( p );
    return f;
}

