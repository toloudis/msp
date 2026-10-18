//////////////////////////////////////////////////////////////////////////////
// Converted from HDRLighting.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// HDRLighting.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

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
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/


//-----------------------------------------------------------------------------
// Global constants
//-----------------------------------------------------------------------------
static const int    MAX_SAMPLES            = 25;    // Maximum texture grabs

// The per-color weighting to be used for luminance calculations in RGB order.
// oops! this is the more accepted value, and it is commented out! why?
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
//
// Explicit registers keep the binding layout identical for every entry point
// (and map directly onto a DX12 root signature / Vulkan descriptor set).
// The initial values these had in HDRLighting.fx are in HDRLighting.effect.json.
//-----------------------------------------------------------------------------
cbuffer HDRParams : register(b0)
{
	// Contains sampling offsets used by the techniques
	float2 g_avSampleOffsets[MAX_SAMPLES];
	float4 g_avSampleWeights[MAX_SAMPLES];

	float4 g_ColorTint;
	float4 g_scaledCopyUVs; // top, bottom, left, right

	float  BRIGHT_PASS_THRESHOLD;	// Threshold for BrightPass filter
	float  BRIGHT_PASS_OFFSET;		// Offset for BrightPass filter

	// Tone mapping variables
	float  g_fMiddleGray;	// The middle gray key value (0.18 in Reinhard paper)
	float  g_fWhiteCutoff;	// Lowest luminance which is mapped to white
	//float  g_fElapsedTime;	// Time in seconds since the last calculation

	//bool  g_bEnableBlueShift;   // Flag indicates if blue shift is performed
	bool  g_bEnableToneMap;     // Flag indicates if tone mapping is performed

	float  g_fBloomScale;       // Bloom process multiplier
	float  g_fStarScale;        // Star process multiplier

	float g_fixedLuminance;

	int g_nSrcComponent;
	int g_nDstComponent;

	bool g_bIsRefOn;
	float g_fRef;

	float g_scaledCopyFactor;

	// LinearMapping range
	float g_RangeMax;
	float g_RangeMin;
};

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------

Texture2D s0 : register(t0);
Texture2D s1 : register(t1);
Texture2D s2 : register(t2);
Texture2D s3 : register(t3);
Texture2D s4 : register(t4);
Texture2D s5 : register(t5);
Texture2D s6 : register(t6);
Texture2D s7 : register(t7);
Texture2DMS<float> TexMS : register(t8);

// this is for pixel processing of quad textures.  
// filter should be either linear or point
// (sampler states are described in HDRLighting.effect.json)
SamplerState g_DefaultSampler : register(s0);

SamplerState g_PointSampler : register(s1);
SamplerState g_PointClampSampler : register(s2);
SamplerState g_LinearSampler : register(s3);
SamplerState g_LinearClampSampler : register(s4);

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

//-----------------------------------------------------------------------------
// Vertex shaders
//-----------------------------------------------------------------------------

struct VS_OUTPUT
{
   float4 Pos: SV_POSITION;
   float2 img: TEXCOORD0;
};

VS_OUTPUT VSMain(float4 Pos: SV_POSITION, float2 UV : TEXCOORD0 )
{
	VS_OUTPUT Out;

	// Clean up inaccuracies
	//Pos.xy = sign(Pos.xy);

	Out.Pos = Pos;//float4(Pos.xy, 0, 1);
	Out.img = UV;
//	Out.img = 0.5 + 0.5*float2(Pos.x, -Pos.y);
	//Out.img.x = 0.5 * (1 + Pos.x);
	//Out.img.y = 0.5 * (1 - Pos.y);

	return Out;
}


//-----------------------------------------------------------------------------
// Pixel shaders
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: SampleLumInitial
// Type: Pixel shader                                      
// Desc: Sample the luminance of the source image using a kernal of sample
//       points, and return a scaled image containing the log() of averages
//-----------------------------------------------------------------------------
float4 SampleLumInitial
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
    float3 vSample = 0.0f;
    float  fLogLumSum = 0.0f;

    for(int iSample = 0; iSample < 9; iSample++)
    {
        // Compute the sum of log(luminance) throughout the sample points
        vSample = s0.Sample(g_LinearSampler, vScreenPosition+g_avSampleOffsets[iSample]).xyz;
		//vSample = CheckNan3(vSample);
        fLogLumSum += (log( GetLuminance(vSample) + 0.0001f));
    }
    
    // Divide the sum to complete the average
    fLogLumSum /= 9.0f;

    return float4(fLogLumSum, fLogLumSum, fLogLumSum, 1.0f);
}

float4 LuminanceToGray
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
	float4 pixSample = s0.Sample(g_DefaultSampler, vScreenPosition);
	float lum = GetLuminance(pixSample.rgb);
	float slum = lum / (1.0f + lum);
    return float4(slum,slum,slum, 1.0f);
}


//-----------------------------------------------------------------------------
// Name: SampleLumIterative
// Type: Pixel shader                                      
// Desc: Scale down the luminance texture by blending sample points
//-----------------------------------------------------------------------------
float4 SampleLumIterative
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
    float fResampleSum = 0.0f; 
    
    for(int iSample = 0; iSample < 16; iSample++)
    {
        // Compute the sum of luminance throughout the sample points
        fResampleSum += (s0.Sample(g_DefaultSampler, vScreenPosition+g_avSampleOffsets[iSample]).x);
    }
    
    // Divide the sum to complete the average
    fResampleSum /= 16.0f;

    return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);
}


//-----------------------------------------------------------------------------
// Name: SampleLumFinal
// Type: Pixel shader                                      
// Desc: Extract the average luminance of the image by completing the averaging
//       and taking the exp() of the result
//-----------------------------------------------------------------------------
float4 SampleLumFinal
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
    float fResampleSum = 0.0f;
    
    for(int iSample = 0; iSample < 16; iSample++)
    {
        // Compute the sum of luminance throughout the sample points
        fResampleSum += (s0.Sample(g_DefaultSampler, vScreenPosition+g_avSampleOffsets[iSample]).x);
    }
    
    // Divide the sum to complete the average, and perform an exp() to complete
    // the average luminance calculation
    fResampleSum = exp(fResampleSum/16.0f);
    
    return float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);
}


//-----------------------------------------------------------------------------
// Name: CalculateAdaptedLum
// Type: Pixel shader                                      
// Desc: Calculate the luminance that the camera is current adapted to, using
//       the most recented adaptation level, the current scene luminance, and
//       the time elapsed since last calculated
//-----------------------------------------------------------------------------
float4 CalculateAdaptedLumPS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
//	float fAdaptedLum = s0.Sample(g_DefaultSampler, float2(0.5f, 0.5f));
	float fCurrentLum = s1.Sample(g_DefaultSampler, float2(0.5f, 0.5f)).x;
    
    float4 retval = float4(fCurrentLum, fCurrentLum, fCurrentLum, 1.0f);
//	if (g_bAdaptiveLuminance)
//	{
		// The user's adapted luminance level is simulated by closing the gap between
		// adapted luminance and current luminance by 2% every frame, based on a
		// 30 fps rate. This is not an accurate model of human adaptation, which can
		// take longer than half an hour.
//		float fNewAdaptation = fAdaptedLum + (fCurrentLum - fAdaptedLum) * ( 1 - pow( 0.98f, 30 * g_fElapsedTime ) );
//		retval = float4(fNewAdaptation, fNewAdaptation, fNewAdaptation, 1.0f);
//	}
	return retval;
}

float4 PS_Reinhard02( float4 hdrColorSample, float avgLuminance, 
	float exposure, float whitePoint )
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

//-----------------------------------------------------------------------------
// Name: FinalScenePass
// Type: Pixel shader                                      
// Desc: Perform blue shift, tone map the scene, and add post-processed light
//       effects
//-----------------------------------------------------------------------------
float4 FinalScenePassPS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
    float4 vSample = s0.Sample(g_PointSampler, vScreenPosition);
    float4 vBloom = s1.Sample(g_LinearClampSampler, vScreenPosition);
    float4 vStar = s2.Sample(g_LinearClampSampler, vScreenPosition);

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
float4 FinalScenePass_FastPS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
    float4 vSample = s0.Sample(g_PointSampler, vScreenPosition);

    // first, put negative pixels to 0.
	vSample = max(vSample, 0);
//	if (vSample.r < 0) vSample.r = 0;
//	if (vSample.g < 0) vSample.g = 0;
//	if (vSample.b < 0) vSample.b = 0;
//	if (vSample.a < 0) vSample.a = 0;
    
    // Very simple mapping of the high range of color values into a range 
    // appropriate for display.
    if( g_bEnableToneMap )
    {
		// map pixel to luminance space
		float3 Yxy = RGB2Yxy(vSample.rgb);
		
		float Lp = Yxy.r/(1.0f+Yxy.r);
		Yxy.r = Lp;

		// map pixel back to RGB space
		vSample.rgb = Yxy2RGB(Yxy);
		vSample.a = clamp(vSample.a, 0.0f, 1.0f);
		//vSample.a = 1.0f;
    }  
    
    return vSample;
}

float4 ScaledCopyPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
	float top = (g_scaledCopyUVs.x + 1) / 2;
	float bottom = (g_scaledCopyUVs.y + 1) / 2;
	float left = (g_scaledCopyUVs.z + 1) / 2;
	float right = (g_scaledCopyUVs.w + 1) / 2;
	
	float2 samplePoint = float2( left + (right-left) * vScreenPosition.x  , top + (bottom-top) * vScreenPosition.y );	
	
	float4 fColor = (s0.Sample(g_DefaultSampler, samplePoint)) * float4(g_scaledCopyFactor,g_scaledCopyFactor,g_scaledCopyFactor,1);
	
    return fColor;
}
float4 SimpleCopyPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    return(s0.Sample(g_DefaultSampler, vScreenPosition));
}
float4 RedChannelPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    return(s0.Sample(g_DefaultSampler, vScreenPosition).rrrr);
}
float4 GreenChannelPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    return(s0.Sample(g_DefaultSampler, vScreenPosition).gggg);
}
float4 BlueChannelPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    return(s0.Sample(g_DefaultSampler, vScreenPosition).bbbb);
}
float4 AlphaChannelPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    return(s0.Sample(g_DefaultSampler, vScreenPosition).aaaa);
}
float4 SimpleCopyInvAlphaPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    float4 fColor = s0.Sample(g_DefaultSampler, vScreenPosition);
    return float4( fColor.rgb, 1-fColor.a );
}
float4 SimpleCopyInvGAlphaPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    float4 fColor = s0.Sample(g_DefaultSampler, vScreenPosition);
    return float4( fColor.rgb, (1-fColor.g) );
}

float4 SimpleCopyLDRPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    float4 fColor = saturate(s0.Sample(g_DefaultSampler, vScreenPosition));
    return fColor;
}

float4 SimpleCopyInvSatPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    float4 fColor = saturate(s0.Sample(g_LinearSampler /*g_DefaultSampler*/, vScreenPosition));
    return float4(1-fColor.rgb, fColor.a);
}

float4 SimpleCopyAlphaSatPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    float4 fColor = s0.Sample(g_DefaultSampler, vScreenPosition);
    return float4(fColor.r, fColor.g, fColor.b, clamp(fColor.a, 0.0f, 1.0f));
}

float4 SimpleCopyAlphaRefPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    float4 fColor = s0.Sample( g_DefaultSampler, vScreenPosition);
    if (g_bIsRefOn)
	{
		if (fColor.a >= g_fRef)
			discard;
	}
    return fColor;
}

float4 SimpleCopyLumPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    float fColor = s0.Sample(g_DefaultSampler, vScreenPosition).x;
    return float4(fColor,fColor,fColor,1.0f);
}

float4 SimpleCopyLumLDRPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    float fColor = saturate(s0.Sample(g_DefaultSampler, vScreenPosition).x);
    return float4(fColor,fColor,fColor,1.0f);
}

float4 SimpleDrawAlpha( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    float4 fColor = s0.Sample(g_DefaultSampler, vScreenPosition);
    return float4(fColor.a, fColor.a, fColor.a, 1.0f);
}

float4 ColorTintPS(in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    return(s0.Sample(g_DefaultSampler, vScreenPosition) * g_ColorTint );
}

float4 ColorTintLumPS(in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
    float fColor = saturate(s0.Sample(g_DefaultSampler, vScreenPosition).x);
    return float4(fColor,fColor,fColor,1.0f) * g_ColorTint;
}

float4 CopyComponentPS(in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
	float4 clr = (float4)0;

	float val = 0;
	if( g_nSrcComponent == 0 ) val = s0.Sample(g_DefaultSampler, vScreenPosition).r;
	else if( g_nSrcComponent == 1 ) val = s0.Sample(g_DefaultSampler, vScreenPosition).g;
	else if( g_nSrcComponent == 2 ) val = s0.Sample(g_DefaultSampler, vScreenPosition).b;
	else if( g_nSrcComponent == 3 ) val = s0.Sample(g_DefaultSampler, vScreenPosition).a;

	if( g_nDstComponent == 0 ) clr.r = val;
	else if( g_nDstComponent == 1 ) clr.g = val;
	else if( g_nDstComponent == 2 ) clr.b = val;
	else if( g_nDstComponent == 3 ) clr.a = val;

    return clr;
}

float4 SimpleColorPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 ) : SV_TARGET
{
	return g_ColorTint;
}

struct DepthOutput
{
	float4 Dummy : SV_TARGET;
	float  Depth : SV_DEPTH;
};

DepthOutput DepthCopyPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 )
{
	DepthOutput Out = (DepthOutput)0;

	Out.Depth = saturate( s0.Sample( g_DefaultSampler, vScreenPosition).r );
	return Out;
}

DepthOutput DepthCopyMSPS( in float4 vPos : SV_POSITION, in float2 vScreenPosition : TEXCOORD0 )
{
	DepthOutput Out = (DepthOutput)0;

	Out.Depth = saturate( TexMS.Load( (int2)vPos.xy, 0 ).r );
	return Out;
}

//-----------------------------------------------------------------------------
// Name: DownScale4x4
// Type: Pixel shader                                      
// Desc: Scale the source texture down to 1/16 scale
//-----------------------------------------------------------------------------
float4 DownScale4x4PS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
	
    float4 sample = 0.0f;

	for( int i=0; i < 16; i++ )
	{
		sample += max(0,/*CheckNan4*/(s0.Sample(g_PointClampSampler, vScreenPosition + g_avSampleOffsets[i] )));
	}
    
	return sample / 16.0f;
}


//-----------------------------------------------------------------------------
// Name: DownScale2x2
// Type: Pixel shader                                      
// Desc: Scale the source texture down to 1/4 scale
//-----------------------------------------------------------------------------
float4 DownScale2x2PS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
	
    float4 sample = 0.0f;

	for( int i=0; i < 4; i++ )
	{
		sample += s0.Sample(g_PointClampSampler, vScreenPosition + g_avSampleOffsets[i] );
	}
    
	return sample / 4;
}


//-----------------------------------------------------------------------------
// Name: GaussBlur5x5
// Type: Pixel shader                                      
// Desc: Simulate a 5x5 kernel gaussian blur by sampling the 12 points closest
//       to the center point.
//-----------------------------------------------------------------------------
float4 GaussBlur5x5PS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
	
    float4 sample = 0.0f;

	for( int i=0; i < 25; i++ )
	{
		sample += g_avSampleWeights[i] * s0.Sample(g_DefaultSampler, vScreenPosition + g_avSampleOffsets[i] );
	}

	return sample;
}


//-----------------------------------------------------------------------------
// Name: BrightPassFilter
// Type: Pixel shader                                      
// Desc: Perform a high-pass filter on the source texture
//-----------------------------------------------------------------------------
float4 BrightPassFilterPS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : COLOR
{
	float4 vSample = s0.Sample(g_DefaultSampler, vScreenPosition );
	
	// alternately:
	// convert rgb to luminance (xyz) space
	// subtract out luminance
	// return to rgb space
	// (that means use the more robust tone mapper in here)

    float fAdaptedLum = g_fixedLuminance;
//	if (g_bAdaptiveLuminance)
//		fAdaptedLum = tex2D(s1, float2(0.5f, 0.5f));

	// Determine what the pixel's value will be after tone-mapping occurs
	vSample.rgb *= g_fMiddleGray/(fAdaptedLum + 0.001f);

	// Subtract out dark pixels
	vSample.rgb -= BRIGHT_PASS_THRESHOLD;

	// Clamp to 0
	vSample = max(vSample, 0.0f);

	// Map the resulting value into the 0 to 1 range. Higher values for
	// BRIGHT_PASS_OFFSET will isolate lights from illuminated scene
	// objects.
	vSample.rgb /= (BRIGHT_PASS_OFFSET + vSample.rgb);

	return vSample;
}

float4 BrightPassFilter_Reinhard
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
	float4 vSample = s0.Sample(g_DefaultSampler, vScreenPosition );
	
	// alternately:
	// convert rgb to luminance (xyz) space
	// subtract out luminance
	// return to rgb space
	// (that means use the more robust tone mapper in here)

    float fAdaptedLum = g_fixedLuminance;
//	if (g_bAdaptiveLuminance)
//		fAdaptedLum = tex2D(s1, float2(0.5f, 0.5f));

	float3 Yxy = RGB2Yxy(vSample.rgb);
    
	// (Lp) Map average luminance to the middlegrey zone by scaling pixel luminance
	float Lp = Yxy.r * g_fMiddleGray / fAdaptedLum;                       
	// Subtract out dark pixels
	Lp -= BRIGHT_PASS_THRESHOLD;
	// Clamp to 0
	Lp = max(Lp, 0.0f);
	// Map the resulting value into the 0 to 1 range. Higher values for
	// BRIGHT_PASS_OFFSET will isolate lights from illuminated scene
	// objects.
	Yxy.r = (Lp * (1.0f + Lp/(g_fWhiteCutoff * g_fWhiteCutoff)))/(BRIGHT_PASS_OFFSET + Lp);

	// now map the sample back to a color with the given luminance
	vSample.rgb = Yxy2RGB(Yxy);
	return vSample;
}


//-----------------------------------------------------------------------------
// Name: Bloom
// Type: Pixel shader
// Desc: Blur the source image along one axis using a gaussian
//       distribution. Since gaussian blurs are separable, this shader is called
//       twice; first along the horizontal axis, then along the vertical axis.
//-----------------------------------------------------------------------------
float4 BloomPS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
    
    float4 vSample = 0.0f;
    float4 vColor = 0.0f;
        
    float2 vSamplePosition;
    
    // Perform a one-directional gaussian blur
    for(int iSample = 0; iSample < 25; iSample++)
    {
        vSamplePosition = vScreenPosition + g_avSampleOffsets[iSample];
        vColor = s0.Sample(g_DefaultSampler, vSamplePosition);
        vSample += g_avSampleWeights[iSample]*vColor;
    }
    
    return vSample;
}


//-----------------------------------------------------------------------------
// Name: Star
// Type: Pixel shader                                      
// Desc: Each star is composed of up to 8 lines, and each line is created by
//       up to three passes of this shader, which samples from 8 points along
//       the current line.
//-----------------------------------------------------------------------------
float4 StarPS
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
    float4 vSample = 0.0f;
    float4 vColor = 0.0f;
        
    float2 vSamplePosition;
    
    // Sample from eight points along the star line
    for(int iSample = 0; iSample < 8; iSample++)
    {
        vSamplePosition = vScreenPosition + g_avSampleOffsets[iSample];
        vSample = s0.Sample(g_LinearSampler, vSamplePosition);
        vColor += g_avSampleWeights[iSample] * vSample;
    }
    	
    return vColor;
}


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Pixel shader                                      
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------
float4 MergeTextures_1PS
	(
    in float4 vPos : SV_POSITION,
	in float2 vScreenPosition : TEXCOORD0
	) : SV_TARGET
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * s0.Sample(g_LinearSampler, vScreenPosition);
		
	return vColor;
}


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Pixel shader                                      
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------
float4 MergeTextures_2PS
	(
    in float4 vPos : SV_POSITION,
	in float2 vScreenPosition : TEXCOORD0
	) : SV_TARGET
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * s0.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[1] * s1.Sample(g_LinearSampler, vScreenPosition);
		
	return vColor;
}


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Pixel shader                                      
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------
float4 MergeTextures_3PS
	(
    in float4 vPos : SV_POSITION,
	in float2 vScreenPosition : TEXCOORD0
	) : SV_TARGET
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * s0.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[1] * s1.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[2] * s2.Sample(g_LinearSampler, vScreenPosition);
		
	return vColor;
}


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Pixel shader                                      
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------
float4 MergeTextures_4PS
	(
    in float4 vPos : SV_POSITION,
	in float2 vScreenPosition : TEXCOORD0
	) : SV_TARGET
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * s0.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[1] * s1.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[2] * s2.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[3] * s3.Sample(g_LinearSampler, vScreenPosition);
		
	return vColor;
}


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Pixel shader                                      
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------
float4 MergeTextures_5PS
	(
    in float4 vPos : SV_POSITION,
	in float2 vScreenPosition : TEXCOORD0
	) : SV_TARGET
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * s0.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[1] * s1.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[2] * s2.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[3] * s3.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[4] * s4.Sample(g_LinearSampler, vScreenPosition);
		
	return vColor;
}


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Pixel shader                                      
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------
float4 MergeTextures_6PS
	(
    in float4 vPos : SV_POSITION,
	in float2 vScreenPosition : TEXCOORD0
	) : SV_TARGET
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * s0.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[1] * s1.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[2] * s2.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[3] * s3.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[4] * s4.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[5] * s5.Sample(g_LinearSampler, vScreenPosition);
		
	return vColor;
}


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Pixel shader                                      
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------
float4 MergeTextures_7PS
	(
    in float4 vPos : SV_POSITION,
	in float2 vScreenPosition : TEXCOORD0
	) : SV_TARGET
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * s0.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[1] * s1.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[2] * s2.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[3] * s3.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[4] * s4.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[5] * s5.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[6] * s6.Sample(g_LinearSampler, vScreenPosition);
		
	return vColor;
}


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Pixel shader                                      
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------
float4 MergeTextures_8PS
	(
    in float4 vPos : SV_POSITION,
	in float2 vScreenPosition : TEXCOORD0
	) : SV_TARGET
{
	float4 vColor = 0.0f;
	
	vColor += g_avSampleWeights[0] * s0.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[1] * s1.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[2] * s2.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[3] * s3.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[4] * s4.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[5] * s5.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[6] * s6.Sample(g_LinearSampler, vScreenPosition);
	vColor += g_avSampleWeights[7] * s7.Sample(g_LinearSampler, vScreenPosition);
		
	return vColor;
}


//-----------------------------------------------------------------------------
// Name: LinearMapping
// Type: Pixel shader                                      
// Desc: Map values as a percentage of a range
//-----------------------------------------------------------------------------
// g_RangeMax and g_RangeMin are in the HDRParams cbuffer
float4 LinearMappingPS
	(
    in float4 vPos : SV_POSITION,
	in float2 vScreenPosition : TEXCOORD0
	) : SV_TARGET
{
	
	// assumes values are in the R component!
	
	float4 vColor = s0.Sample(g_DefaultSampler, vScreenPosition).r;
	
	// near = 1, far = 0
	vColor = saturate(1.0 - (vColor-g_RangeMin)/(g_RangeMax-g_RangeMin));		
	return vColor;
}


//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: Bloom
// Type: Technique                                     
// Desc: Performs a single horizontal or vertical pass of the blooming filter
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: Star
// Type: Technique                                     
// Desc: Perform one of up to three passes composing the current star line
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: SampleAvgLum
// Type: Technique                                     
// Desc: Takes the HDR Scene texture as input and starts the process of 
//       determining the average luminance by converting to grayscale, taking
//       the log(), and scaling the image to a single pixel by averaging sample 
//       points.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: ResampleAvgLum
// Type: Technique                                     
// Desc: Continue to scale down the luminance texture
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: ResampleAvgLumExp
// Type: Technique                                     
// Desc: Sample the texture to a single pixel and perform an exp() to complete
//       the evalutation
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: CalculateAdaptedLum
// Type: Technique                                     
// Desc: Determines the level of the user's simulated light adaptation level
//       using the last adapted level, the current scene luminance, and the
//       time since last calculation
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: DownScale4x4
// Type: Technique                                     
// Desc: Scale the source texture down to 1/16 scale
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: DownScale2x2
// Type: Technique                                     
// Desc: Scale the source texture down to 1/4 scale
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: GaussBlur5x5
// Type: Technique                                     
// Desc: Simulate a 5x5 kernel gaussian blur by sampling the 12 points closest
//       to the center point.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: BrightPassFilter
// Type: Technique                                     
// Desc: Perform a high-pass filter on the source texture
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: FinalScenePass
// Type: Technique                                     
// Desc: Minimally transform and texture the incoming geometry
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: DepthCopy
// Type: Technique                                     
// Desc: returns a copy of the source depth buffer into the target depths
// Note: Must have Color Writes Disabled
//-----------------------------------------------------------------------------


//multisample version


//-----------------------------------------------------------------------------
// Name: ColorTint
// Type: Technique                                     
// Desc: returns the modulation of the source texture with the tint value
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: ColorTintLum
// Type: Technique                                     
// Desc: Return the modulation of the source texture luminance with the tint value
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: CopyComponent
// Type: Technique                                     
// Desc: returns the copy of the src component into the dest component
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: SimpleColor
// Type: Technique                                     
// Desc: returns the color specified in g_ColorTint, used to fill the target with a color
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Technique                                     
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Technique                                     
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Technique                                     
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Technique                                     
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Technique                                     
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Technique                                     
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Technique                                     
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: MergeTextures_N
// Type: Technique                                     
// Desc: Return the average of N input textures
//-----------------------------------------------------------------------------
