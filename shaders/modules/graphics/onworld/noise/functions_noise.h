// NO TRIG VARIATIONS: https://www.shadertoy.com/view/4djSRW

const float HASHSCALE1 = 0.1031;

float OW_Noise_Hash1To1NoSin(float p)
{
    float3 p3 = frac(p.xxx * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

float OW_Noise_Hash2To1NoSin(in float2 p)
{
    float3 p3 = frac(p.xyx * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

float OW_Noise_Hash3To1NoSin(in float3 p3)
{
    p3 = frac(p3 * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

// ---------------------------------

float OW_Noise_Hash1To1(in float p)
{
    return frac(sin(p) * 43758.5453123f);
}

float OW_Noise_Hash2To1(in float2 p)
{
    float h = dot(p,float2(127.1f,311.7f));
    return frac(sin(h) * 43758.5453123f);
}

float OW_Noise_Hash3To1(float3 p)
{
	p  = frac(p * float3(.1031,.11369,.13787));
	p += dot(p, p.yzx + 19.19f);
	return frac((p.x + p.y) * p.z);
}

float3 OW_Noise_Hash1To3(float p)
{
    float3 h = float3(1275.231f, 4461.7f, 7182.423f) * p;	
    return frac(sin(h) * 43758.5453123f);
}

// https://www.shadertoy.com/view/4djSRW

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

// This is a direct port from Webgl-noise GLSL implementation to HLSL
// https://github.com/ashima/webgl-noise/blob/master/src/noise3D.glsl
float3 mod289_2(float3 x)
{
  return x - floor(x * (1.0f / 289.0f)) * 289.0f;
}

float4 mod289_2(float4 x)
{
  return x - floor(x * (1.0f / 289.0f)) * 289.0f;
}


float4 permute_2(float4 x) 
{
     return mod289_2(((x * 34.0f) + 1.0f) * x);
}

float3 mod289_3(float3 x)
{
  return x - floor(x * (1.0f / 289.0f)) * 289.0f;
}

float3 permute_3(float3 x) 
{
     return mod289_3(((x * 34.0f) + 1.0f) * x);
}

float4 taylorInvSqrt_2(float4 r)
{
    return 1.79284291400159f - 0.85373472095314f * r;
}

float snoise_2(float3 v)
{
    const float2  C = float2(1.0f/6.0f, 1.0f/3.0f);
    const float4  D = float4(0.0f, 0.5f, 1.0f, 2.0f);

    // First corner
    float3 i  = floor(v + dot(v, C.yyy));
    float3 x0 = v - i + dot(i, C.xxx);

    // Other corners
    float3 g = step(x0.yzx, x0.xyz);
    float3 l = 1.0 - g;
    float3 i1 = min( g.xyz, l.zxy );
    float3 i2 = max( g.xyz, l.zxy );

    //   x0 = x0 - 0.0f + 0.0f * C.xxx;
    //   x1 = x0 - i1  + 1.0f * C.xxx;
    //   x2 = x0 - i2  + 2.0f * C.xxx;
    //   x3 = x0 - 1.0f + 3.0f * C.xxx;
    float3 x1 = x0 - i1 + C.xxx;
    float3 x2 = x0 - i2 + C.yyy; // 2.0f*C.x = 1/3 = C.y
    float3 x3 = x0 - D.yyy;      // -1.0f+3.0f*C.x = -0.5f = -D.y

    // Permutations
    i = mod289_2(i); 
    float4 p = permute_2( permute_2( permute_2( 
             i.z + float4(0.0f, i1.z, i2.z, 1.0f ))
           + i.y + float4(0.0f, i1.y, i2.y, 1.0f )) 
           + i.x + float4(0.0f, i1.x, i2.x, 1.0f ));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857f; // 1.0/7.0
    float3  ns = n_ * D.wyz - D.xzx;

    float4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

    float4 x_ = floor(j * ns.z);
    float4 y_ = floor(j - 7.0f * x_ );    // mod(j,N)

    float4 x = x_ *ns.x + ns.yyyy;
    float4 y = y_ *ns.x + ns.yyyy;
    float4 h = 1.0f - abs(x) - abs(y);

    float4 b0 = float4( x.xy, y.xy );
    float4 b1 = float4( x.zw, y.zw );

    //float4 s0 = float4(lessThan(b0,0.0f))*2.0f- 1.0f;
    //float4 s1 = float4(lessThan(b1,0.0f))*2.0f - 1.0f;
    float4 s0 = floor(b0)*2.0f + 1.0f;
    float4 s1 = floor(b1)*2.0f + 1.0f;
    float4 sh = -step(h, float4(0.0, 0.0f, 0.0f, 0.0f));

    float4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    float4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

    float3 p0 = float3(a0.xy,h.x);
    float3 p1 = float3(a0.zw,h.y);
    float3 p2 = float3(a1.xy,h.z);
    float3 p3 = float3(a1.zw,h.w);

    //Normalise gradients
    float4 norm = taylorInvSqrt_2(float4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    float4 m = max(0.6f - float4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0f);
    m = m * m;
    return 42.0f * dot( m*m, float4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}