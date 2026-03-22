
// Derived from batch testing
const int IEEE_INT_RCP_CONST_NR0 = 0x7EF311C2; 
const int IEEE_INT_RCP_CONST_NR1 = 0x7EF311C3;
const int IEEE_INT_RCP_CONST_NR2 = 0x7EF312AC;

// Derived from batch testing
const int IEEE_INT_SQRT_CONST_NR0 = 0x1FBD1DF5;

// Biases for global ranges
// 0-1 or 1-2 specific ranges might improve from different bias
// Derived from batch testing
const int IEEE_INT_RCP_SQRT_CONST_NR0 = 0x5F37642F;
const int IEEE_INT_RCP_SQRT_CONST_NR1 = 0x5F375A86;
const int IEEE_INT_RCP_SQRT_CONST_NR2 = 0x5F375A86;

//
// RCP SQRT
//

// Approximate guess using integer float arithmetics based on IEEE floating point standard
float rcpSqrtIEEEIntApproximation(float inX, int inRcpSqrtConst)
{
    int x = asint(inX);
        x = inRcpSqrtConst - (x >> 1);
    return asfloat(x);
}

float rcpSqrtNewtonRaphson(float inXHalf, float inRcpX)
{
    return inRcpX * (-inXHalf * (inRcpX * inRcpX) + 1.5f);
}

//
// Using 0 Newton Raphson iterations
// Relative error : ~3.4% over full
// Precise format : ~small float
// 2 ALU
//
float fastRcpSqrtNR0(float inX)
{
    float  xRcpSqrt = rcpSqrtIEEEIntApproximation( inX, IEEE_INT_RCP_SQRT_CONST_NR0 );
    return xRcpSqrt;
}

//
// Using 1 Newton Raphson iterations
// Relative error : ~0.2% over full
// Precise format : ~half float
// 6 ALU
//
float fastRcpSqrtNR1(float inX)
{
    float  xhalf    = 0.5f * inX;
    float  xRcpSqrt = rcpSqrtIEEEIntApproximation( inX, IEEE_INT_RCP_SQRT_CONST_NR1 );
           xRcpSqrt = rcpSqrtNewtonRaphson(xhalf, xRcpSqrt);
    return xRcpSqrt;
}

//
// Using 2 Newton Raphson iterations
// Relative error : ~4.6e-004%  over full
// Precise format : ~full float
// 9 ALU
//
float fastRcpSqrtNR2(float inX)
{
    float  xhalf    = 0.5f * inX;
    float  xRcpSqrt = rcpSqrtIEEEIntApproximation( inX, IEEE_INT_RCP_SQRT_CONST_NR2 );
           xRcpSqrt = rcpSqrtNewtonRaphson(xhalf, xRcpSqrt);
           xRcpSqrt = rcpSqrtNewtonRaphson(xhalf, xRcpSqrt);
    return xRcpSqrt;
}


//
// SQRT
//
float sqrtIEEEIntApproximation(float inX, int inSqrtConst)
{
    int x = asint(inX);
        x = inSqrtConst + (x >> 1);
    return asfloat(x);
}

//
// Using 0 Newton Raphson iterations
// Relative error : < 0.7% over full
// Precise format : ~small float
// 1 ALU
//
float fastSqrtNR0(float inX)
{
    float  xRcp = sqrtIEEEIntApproximation( inX, IEEE_INT_SQRT_CONST_NR0 );
    return xRcp;
}

//
// Use inverse Rcp Sqrt
// Using 1 Newton Raphson iterations
// Relative error : ~0.2% over full
// Precise format : ~half float
// 6 ALU
//
float fastSqrtNR1(float inX)
{
    // Inverse Rcp Sqrt
    return inX * fastRcpSqrtNR1(inX);
}

//
// Use inverse Rcp Sqrt
// Using 2 Newton Raphson iterations
// Relative error : ~4.6e-004%  over full
// Precise format : ~full float
// 9 ALU
//
float fastSqrtNR2(float inX)
{
    // Inverse Rcp Sqrt
    return inX * fastRcpSqrtNR2(inX);
}

//
// RCP
//

float rcpIEEEIntApproximation(float inX, int inRcpConst)
{
    int x = asint(inX);
        x = inRcpConst - x;
    return asfloat(x);
}

float rcpNewtonRaphson(float inX, float inRcpX)
{
    return inRcpX * (-inRcpX * inX + 2.0f);
}

//
// Using 0 Newton Raphson iterations
// Relative error : < 0.4% over full
// Precise format : ~small float
// 1 ALU
//
float fastRcpNR0(float inX)
{
    float  xRcp = rcpIEEEIntApproximation( inX, IEEE_INT_RCP_CONST_NR0 );
    return xRcp;
}

//
// Using 1 Newton Raphson iterations
// Relative error : < 0.02% over full
// Precise format : ~half float
// 3 ALU
//
float fastRcpNR1(float inX)
{
    float  xRcp = rcpIEEEIntApproximation( inX, IEEE_INT_RCP_CONST_NR1 );
           xRcp = rcpNewtonRaphson(inX, xRcp);
    return xRcp;
}

//
// Using 2 Newton Raphson iterations
// Relative error : < 5.0e-005%  over full
// Precise format : ~full float
// 5 ALU
//
float fastRcpNR2(float inX)
{
    float  xRcp = rcpIEEEIntApproximation( inX, IEEE_INT_RCP_CONST_NR2 );
           xRcp = rcpNewtonRaphson(inX, xRcp);
           xRcp = rcpNewtonRaphson(inX, xRcp);
    return xRcp;
}


//
// Trigonometric functions
//
const float fsl_PI                = 3.1415926535897932384626433f;
const float fsl_HALF_PI           = 0.5 * fsl_PI;

// Fast unsafe normalize 6 ALU vs 8 ALU normalize()
float3 normalizeFast(float3 inV)
{
    return inV * rsqrt(dot(inV, inV));
}

// 4th order polynomial approximation
// 4 VGRP, 16 ALU Full Rate
// 7 * 10^-5 radians precision
float acosFast4(float inX)
{
    float x1    = saturate( abs(inX) ); // clamp to 1
    float x2    = x1 * x1;
    float x3    = x2 * x1;
    float s;
    
    // Taylor Series Expansion
    s    = -0.2121144 * x1 + 1.5707288;
    s    =  0.0742610 * x2 + s; 
    s    = -0.0187293 * x3 + s; 
    s    =  sqrt(1.0f - x1) * s;
    
    // acos function mirroring
    return inX > 0.0 ? s : (fsl_PI - s);
}

// 4th order polynomial approximation
// 4 VGRP, 16 ALU Full Rate
// 7 * 10^-5 radians precision 
float asinFast4(float inX)
{
    return fsl_HALF_PI - acosFast4(inX);
}

// 4th order hyperbolical approximation
// 4 VGRP, 12 ALU Full Rate
// 7 * 10^-5 radians precision 
float atanFast4(float inX)
{
    float  x = inX;
    return x*(-0.1784 * abs(x) - 0.0663 * x * x + 1.0301);
}
