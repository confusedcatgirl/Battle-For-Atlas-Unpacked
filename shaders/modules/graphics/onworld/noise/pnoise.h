// MR_balsbalsd


// const float foo = 2.0f;

float perm(float x)
{
    const int p[] ={151,160,137,91,90,15,
                    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
                    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
                    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
                    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
                    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
                    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
                    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
                    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
                    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
                    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
                    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
                    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

  return p[fmod(x, 256.0f)];
}

float grad(float x, float3 p)
{
    const float3 g[] = {float3(1,1,0),   float3(-1,1,0),    float3(1,-1,0),    float3(-1,-1,0),
                        float3(1,0,1),    float3(-1,0,1),    float3(1,0,-1),    float3(-1,0,-1),
                        float3(0,1,1),    float3(0,-1,1),    float3(0,1,-1),    float3(0,-1,-1),
                        float3(1,1,0),    float3(0,-1,1),    float3(-1,1,0),    float3(0,-1,-1)};

    return dot(g[fmod(x, 16)], p);
}

float OW_Noise_Perlin3D( float3 p, float repeat)
{
    int3 I = fmod(floor(p), repeat);
    int3 J = (I+1) % repeat.xxx;
    // I += base;  
    // J += base;  
  
  p -= floor(p);  

  float3 f = p * p * p * (p * (p * 6 - 15) + 10);

    int A  = perm(I.x);  
    int AA = perm(A+I.y);  
    int AB = perm(A+J.y);  
  
    int B  =  perm(J.x);  
    int BA = perm(B+I.y);  
    int BB = perm(B+J.y);  
  
    return lerp( lerp( lerp( grad(perm(AA+I.z), p + float3( 0,  0,  0)) ,  
                                 grad(perm(BA+I.z), p + float3(-1,  0,  0) ), f.x),  
                           lerp( grad(perm(AB+I.z), p + float3( 0, -1,  0) ),  
                                 grad(perm(BB+I.z), p + float3(-1, -1,  0) ), f.x), f.y),  
                     lerp( lerp( grad(perm(AA+J.z), p + float3( 0,  0, -1) ),  
                                 grad(perm(BA+J.z), p + float3(-1,  0, -1) ), f.x),  
                           lerp( grad(perm(AB+J.z), p + float3( 0, -1, -1) ),  
                                 grad(perm(BB+J.z), p + float3(-1, -1, -1) ), f.x), f.y), f.z);
}

// float OW_Noise_Perlin3D( float3 p, float period)
// {
//     // p *= 2.0f;
//     // p *= 1.0f / 128.0f;
//     // p *= 128.0f;
//     // p*= 10.0f;
//     p *= 1.0f / 16.0f;
//     // p = fmod(p, 2);

//     // p = fmod(p, period);
//     // p = fmod(p * 128.0f, 1.0f / 128);
//     float3 P = fmod(floor(p), 128.0f * 1.0f / 16.0f);
//     p -= floor(p);

//     float3 f = p * p * p * (p * (p * 6 - 15) + 10);

//     // HASH COORDINATES FOR 6 OF THE 8 CUBE CORNERS
//     float A = perm(P.x, period) + P.y;
//     float AA = perm(A, period) + P.z;
//     float AB = perm(A + 1, period) + P.z;
//     float B =  perm(P.x + 1, period) + P.y;
//     float BA = perm(B, period) + P.z;
//     float BB = perm(B + 1, period) + P.z;

//     // AND ADD BLENDED RESULTS FROM 8 CORNERS OF CUBE
//     return lerp(
//                     lerp(
//                             lerp(grad(perm(AA, period), p), grad(perm(BA, period), p + float3(-1, 0, 0)), f.x), 
//                             lerp(grad(perm(AB, period), p + float3(0, -1, 0)), grad(perm(BB, period), p + float3(-1, -1, 0)), f.x), 
//                             f.y), 
//                     lerp(
//                             lerp(grad(perm(AA + 1, period), p + float3(0, 0, -1)), grad(perm(BA + 1, period), p + float3(-1, 0, -1)), f.x), 
//                             lerp(grad(perm(AB + 1, period), p + float3(0, -1, -1)), grad(perm(BB + 1, period), p + float3(-1, -1, -1)), f.x), 
//                             f.y), 
//                     f.z);
// }
// float OW_Noise_Perlin3D( float3 P, float period)
// {
//     //  https://github.com/BrianSharpe/Wombat/blob/master/Perlin3D.glsl
//     // establish our grid cell and unit position
//     // P = fmod(P, period);
//     float3 Pi = floor(P);
//     float3 Pf = P - Pi;
//     float3 Pf_min1 = Pf - 1.0;

//     // clamp the domain
//     Pi.xyz = Pi.xyz - floor(Pi.xyz * ( 1.0 / period )) * period;
//     float3 Pi_inc1 = step( Pi, float3( period - 1.5, period - 1.5, period - 1.5 ) ) * ( Pi + 1.0 );

//     // calculate the hash
//     float4 Pt = float4( Pi.xy, Pi_inc1.xy ) + float2( 50.0, 161.0 ).xyxy;
//     Pt *= Pt;
//     Pt = Pt.xzxz * Pt.yyww;
//     const float3 SOMELARGEFLOATS = float3( 635.298681, 682.357502, 668.926525 );
//     const float3 ZINC = float3( 48.500388, 65.294118, 63.934599 );
//     float3 lowz_mod = float3( 1.0 / ( SOMELARGEFLOATS + Pi.zzz * ZINC ) );
//     float3 highz_mod = float3( 1.0 / ( SOMELARGEFLOATS + Pi_inc1.zzz * ZINC ) );
//     float4 hashx0 = frac( Pt * lowz_mod.xxxx );
//     float4 hashx1 = frac( Pt * highz_mod.xxxx );
//     float4 hashy0 = frac( Pt * lowz_mod.yyyy );
//     float4 hashy1 = frac( Pt * highz_mod.yyyy );
//     float4 hashz0 = frac( Pt * lowz_mod.zzzz );
//     float4 hashz1 = frac( Pt * highz_mod.zzzz );

//     // calculate the gradients
//     float4 grad_x0 = hashx0 - 0.49999;
//     float4 grad_y0 = hashy0 - 0.49999;
//     float4 grad_z0 = hashz0 - 0.49999;
//     float4 grad_x1 = hashx1 - 0.49999;
//     float4 grad_y1 = hashy1 - 0.49999;
//     float4 grad_z1 = hashz1 - 0.49999;
//     float4 grad_results_0 = rsqrt( grad_x0 * grad_x0 + grad_y0 * grad_y0 + grad_z0 * grad_z0 ) * ( float2( Pf.x, Pf_min1.x ).xyxy * grad_x0 + float2( Pf.y, Pf_min1.y ).xxyy * grad_y0 + Pf.zzzz * grad_z0 );
//     float4 grad_results_1 = rsqrt( grad_x1 * grad_x1 + grad_y1 * grad_y1 + grad_z1 * grad_z1 ) * ( float2( Pf.x, Pf_min1.x ).xyxy * grad_x1 + float2( Pf.y, Pf_min1.y ).xxyy * grad_y1 + Pf_min1.zzzz * grad_z1 );

//     // Classic Perlin Interpolation
//     float3 blend = Pf * Pf * Pf * (Pf * (Pf * 6.0 - 15.0) + 10.0);
//     float4 res0 = lerp( grad_results_0, grad_results_1, blend.z );
//     float4 blend2 = float4( blend.xy, float2( 1.0 - blend.xy ) );
//     float final = dot( res0, blend2.zxzx * blend2.wwyy );
//     return ( final * 1.1547005383792515290182975610039 );  // scale things to a strict -1.0->1.0 range  *= 1.0/sqrt(0.75)
// }

float OW_Noise_Perlin3D_FBM(float3 P, float octavesCount, float lacunarity, float frequency)
{
    const float gain = 1.0f / lacunarity;
    float amplitude = gain;
    float noiseValue = 0.0f;
    for (float i = 0.0f; i < octavesCount; ++i)
    {
        noiseValue += OW_Noise_Perlin3D(P * frequency, frequency) * amplitude;

        frequency *= lacunarity;
        amplitude *= gain;
    }

    return noiseValue;
}