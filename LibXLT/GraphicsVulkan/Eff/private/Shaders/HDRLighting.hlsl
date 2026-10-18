/*****************************************************************************
**  HDRLighting.fx
**
** Hugely derived from Microsoft HDRLighting sample app. 
** Many user controls added and tone mapping function improved.
**
** Desc: Effect file for High Dynamic Range Lighting sample. This file contains
**       shaders used to quickly calculate the average luminance of the
**       rendered scene, simulate the viewer's light adaptation level, map the
**       high dynamic range of colors to a range displayable on a PC monitor,
**       and perform post-process lighting effects.
**
** The algorithms described in this sample are based very closely on the
** lighting effects implemented in Masaki Kawase's Rthdribl sample and the tone
** mapping process described in the whitepaper "Tone Reproduction for Digital
** Images"
**
** Real-Time High Dynamic Range Image-Based Lighting (Rthdribl)
** Masaki Kawase
** http://www.daionet.gr.jp/~masa/rthdribl/
**
** "Photographic Tone Reproduction for Digital Images"
** Erik Reinhard, Mike Stark, Peter Shirley and Jim Ferwerda
** http://www.cs.utah.edu/~reinhard/cdrom/
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/


//-----------------------------------------------------------------------------
// Global constants
//-----------------------------------------------------------------------------
static const int    MAX_SAMPLES            = 25;    // Maximum texture grabs

// The per-color weighting to be used for luminance calculations in RGB order.
//static const float3 LUMINANCE_VECTOR  = float3(0.2125f, 0.7154f, 0.0721f);
static const float3 LUMINANCE_VECTOR  = float3(0.265068,  0.67023428, 0.06409157);

// note that the 2nd row is the LUMINANCE_VECTOR 
// RGB -> XYZ conversion
static const float3x3 RGB2XYZ = 
	{0.5141364, 0.3238786,  0.16036376,
	0.265068,  0.67023428, 0.06409157,
	0.0241188, 0.1228178,  0.84442666};
	
// XYZ -> RGB conversion
static const float3x3 XYZ2RGB = 
	{ 2.5651,-1.1665,-0.3986,
	-1.0217, 1.9777, 0.0439,
	0.0753, -0.2543, 1.1892};

// The per-color weighting to be used for blue shift under low light.
static const float3 BLUE_SHIFT_VECTOR = float3(1.05f, 0.97f, 1.27f);


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

// Contains sampling offsets used by the techniques
float2 g_avSampleOffsets[MAX_SAMPLES];
float4 g_avSampleWeights[MAX_SAMPLES];

// Tone mapping variables
cbuffer ToneMap : register(b0)
{
	float  g_fMiddleGray = 1.0;	// The middle gray key value (0.18 in Reinhard paper)
	float  g_fWhiteCutoff = 1.0;	// Lowest luminance which is mapped to white
	//float  g_fElapsedTime = 0.0;	// Time in seconds since the last calculation

	float g_bEnableBlueShift = false;   // Flag indicates if blue shift is performed
	float  g_bEnableToneMap = true;     // Flag indicates if tone mapping is performed

	float  g_fBloomScale = 1.0;       // Bloom process multiplier
	float  g_fStarScale = 0.5;        // Star process multiplier

	float  BRIGHT_PASS_THRESHOLD  = 5.0f;  // Threshold for BrightPass filter
	float  BRIGHT_PASS_OFFSET     = 10.0f; // Offset for BrightPass filter
	
    uint4    g_param;   
};

static const float g_fixedLuminance = 1;

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);
sampler s4 : register(s4);
sampler s5 : register(s5);
sampler s6 : register(s6);
sampler s7 : register(s7);

float CheckNan(float color)
{
	float outcolor = color;
	if (isinf(color))
		outcolor = 1;
	else if (isnan(color))
		outcolor = 0;
	return outcolor;
}

float3 CheckNan3(float3 color)
{
	float3 outcolor = color;
	if (isinf(color.r) || isinf(color.g) || isinf(color.b))
		outcolor = float3(1,0,0);
	else if (isnan(color.r) || isnan(color.g) || isnan(color.b))
		outcolor = float3(0,0,1);
	return outcolor;
}
float4 CheckNan4(float4 color)
{
	float4 outcolor = color;
	if (isinf(color.r) || isinf(color.g) || isinf(color.b) || isinf(color.a))
		outcolor = float4(1,0,0,1);
	else if (isnan(color.r) || isnan(color.g) || isnan(color.b) || isnan(color.a))
		outcolor = float4(0,0,1,1);
	return outcolor;
}

float GetLuminance(float3 rgb)
{
	return dot(rgb, LUMINANCE_VECTOR);
}

// Y is luminance
float3 RGB2Yxy(float3 colorrgb)
{
  float3 XYZ = mul(RGB2XYZ, colorrgb);
  
  // XYZ -> Yxy conversion (Y is luminance in Yxy space)
  float3 Yxy;
  Yxy.r = XYZ.g;                            // copy luminance Y
  Yxy.g = XYZ.r / (XYZ.r + XYZ.g + XYZ.b ); // x = X / (X + Y + Z)
  Yxy.b = XYZ.g / (XYZ.r + XYZ.g + XYZ.b ); // y = Y / (X + Y + Z)
  return Yxy;
}

// Y is luminance
float3 Yxy2RGB(float3 colorYxy)
{
	float3 XYZ;
  // Yxy -> XYZ conversion
  XYZ.r = colorYxy.r * colorYxy.g / colorYxy. b;					// X = Y * x / y
  XYZ.g = colorYxy.r;												// copy luminance Y
  XYZ.b = colorYxy.r * (1 - colorYxy.g - colorYxy.b) / colorYxy.b;	// Z = Y * (1-x-y) / y
    
  return mul(XYZ2RGB, XYZ);
}

float4 LuminanceToGray
    (
    in float2 vScreenPosition : TEXCOORD0
    )
{
	float4 pixSample = tex2D(s0, vScreenPosition);
	float lum = GetLuminance(pixSample.rgb);
	float slum = lum / (1.0f + lum);
    return float4(slum,slum,slum, 1.0f);
}



float4 PS_Reinhard02( float4 hdrColorSample, float avgLuminance, 
	float exposure, float whitePoint ) : COLOR
{
	// map pixel to luminance space
	float3 Yxy = RGB2Yxy(hdrColorSample.rgb);
	// (Lp) Map average luminance to the middlegrey zone by scaling pixel luminance
	float Lp = Yxy.r * exposure / avgLuminance;                       
	// (Ld) Scale all luminance within a displayable range of 0 to 1
	Yxy.r = (Lp * (1.0f + Lp/(whitePoint * whitePoint)))/(1.0f + Lp);
	// map pixel back to RGB space
	hdrColorSample.rgb = Yxy2RGB(Yxy);
	hdrColorSample.a = clamp(hdrColorSample.a, 0.0f, 1.0f);
	//hdrColorSample.a = 1.0f;
	return hdrColorSample;
}

StructuredBuffer<float4> buffer : register( t0 );
struct QuadVS_Output
{
    float4 Pos : SV_POSITION;              
    float2 Tex : TEXCOORD0;
};


//-----------------------------------------------------------------------------
// Name: FinalScenePass
// Type: Pixel shader                                      
// Desc: Perform blue shift, tone map the scene, and add post-processed light
//       effects
//-----------------------------------------------------------------------------
float4 FinalScenePassPS( QuadVS_Output Input ) : SV_TARGET
{
    float4 vSample =  buffer[((1-Input.Tex.x)-0.5)*g_param.x+(1.0 - Input.Tex.y)*g_param.x*g_param.y];
    float4 vBloom = float4(0,0,0,0);//tex2D(s1, vScreenPosition);
    float4 vStar = float4(0,0,0,0);//tex2D(s2, vScreenPosition);

    // first, put negative pixels to 0.
	vSample = max(vSample, 0);
//	if (vSample.r < 0) vSample.r = 0;
//	if (vSample.g < 0) vSample.g = 0;
//	if (vSample.b < 0) vSample.b = 0;
//	if (vSample.a < 0) vSample.a = 0;

    
    float fAdaptedLum = g_fixedLuminance;
//    if (g_bAdaptiveLuminance)
//		fAdaptedLum = tex2D(s3, float2(0.5f, 0.5f));

	// For very low light conditions, the rods will dominate the perception
    // of light, and therefore color will be desaturated and shifted
    // towards blue.
/* // no one is using this; i am disabling it until we have a reason to use it.
    if( g_bEnableBlueShift )
    {
		// Define a linear blending from -1.5 to 2.6 (log scale) which
		// determines the lerp amount for blue shift
        float fBlueShiftCoefficient = 1.0f - (fAdaptedLum + 1.5)/4.1;
        fBlueShiftCoefficient = saturate(fBlueShiftCoefficient);

		// Lerp between current color and blue, desaturated copy
        float3 vRodColor = GetLuminance(vSample.rgb) * BLUE_SHIFT_VECTOR;
        vSample.rgb = lerp( vSample.rgb, vRodColor, fBlueShiftCoefficient );
    }
*/    
	
    // Map the high range of color values into a range appropriate for
    // display, taking into account the user's adaptation level, and selected
    // values for for middle gray and white cutoff.
    if( g_bEnableToneMap )
    {
		vSample = PS_Reinhard02( vSample, fAdaptedLum + 0.0001, g_fMiddleGray, g_fWhiteCutoff );   
  
		//vSample.rgb *= g_fMiddleGray/(fAdaptedLum + 0.0001f);
		//vSample.rgb /= (1.0f+vSample);
    }  
    
    // Add the star and bloom post processing effects
    vSample += float4((g_fStarScale * vStar).rgb,0);
    vSample += float4((g_fBloomScale * vBloom).rgb,0);
    
    return vSample;
}
