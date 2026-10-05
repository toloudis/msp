//////////////////////////////////////////////////////////////////////////////
// Converted from Tessellate.h by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Source of truth for the plain-HLSL material shaders from here on; the
// original Tessellate.h is still used by the Effects (.fx) shaders.
// Register layout: see Globals.hlsli.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Tessellate.h
**
**      ATI Hardware Tessellation functions
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifndef _GLOBALS_
#include "Globals.hlsli"
#endif

#define SGPU_PARTITIONING "fractional_odd"
//#define SGPU_PARTITIONING "integer"

//#define MESHDATATEXTURESIZE	256.0f					//make sure this is a multiple of 16
//#define TEXELSIZE (1.0f/MESHDATATEXTURESIZE)
//#define HALFTEXELSIZE (TEXELSIZE/2.0f)
//const float4 Stride = float4( TEXELSIZE, 0, 0, 0);
#define PATCHSIZE	48		//number of float4 elements for a patch (aka vertex texture stride)
#define UPATCHOFFSET 16		//Offset from the patch index to the first U Tangent patch
#define VPATCHOFFSET 32		//Offset from the patch index to the first V Tangent patch

//#define USETANGENTPATCHES

//this applies the transformation after the tessellation to displace the final vertex
#define APPLY_DISPLACEMENT

//define this to filter the displacement and normals with a bicubic BC Spline
#define BICUBIC_DISPLACEMENT

//this alters the tangent space based on the derived vectors from the displacement map (Sobel Filter)
#define CALCULATE_BUMP_NORMAL

#define BUMP_NORMAL_FALLBACK

// g_IsReflectionGen and g_view are in FrameParams; g_vTessellationFactor,
// the mesh data texture size and the displacement constants are in
// ObjectParams; hasHardwareTessellation is in MaterialCommon (Globals.hlsli).

//stores 16 float4 control points in sequence to define a bi-cubic Bezier patch mesh
Texture2D meshDataTexture : register(t25);		// : meshdatamap

SamplerState bumpMapSampler : register(s2);

//----for displacement mapping---------
Texture2D g_DisplacementMap : register(t3);

SamplerState displacementSampler : register(s3);

SamplerState displacementTapSampler : register(s4);


/*********** structures ******/

struct VS_INPUT_TESS		//triangles
{
	float3 vBarycentric : BLENDWEIGHT0;

	// Superprim Vertex 0
	float4 Position0 : POSITION0;
	float3 Normal0   : NORMAL0;
	float2 UV0		 : TEXCOORD0;
	float3 Tangent0  : TANGENT0;
	float3 Binormal0 : BINORMAL0;

	// Superprim Vertex 1
	float4 Position1 : POSITION4;
	float3 Normal1   : NORMAL4;
	float2 UV1		 : TEXCOORD4;
	float3 Tangent1  : TANGENT4;
	float3 Binormal1 : BINORMAL4;

	// Superprim Vertex 2
	float4 Position2  : POSITION8;
	float3 Normal2    : NORMAL8;
	float2 UV2		  : TEXCOORD8;
	float3 Tangent2   : TANGENT8;
	float3 Binormal2  : BINORMAL8;
};

//Vertex output for passing on to the Hull, Tessellation, or Domain stages
struct TANGENT_TESS_OUTPUT
{
	TANGENT_VERTEX	V;
};

float SampleDisplacementMap( float2 UV )
{
	// Read height
	float height = g_DisplacementMap.SampleLevel(displacementTapSampler, UV, (g_DisplacementBlur*10.0f)-0.5f).x;	//blur handles 10 mip levels
	return (height - g_DisplacementBias) * g_DisplacementScale;
}

float SampleDisplacementMapBilinear( float2 UV, float LOD )
{
	float2 Dims;
	float Mips;
	g_DisplacementMap.GetDimensions( LOD, Dims.x, Dims.y, Mips );

	float MipLevel = LOD-0.5f;	//remove stupid half texel mip level offset

	//remove half texel offset
	float2 f = frac(UV * Dims);  // we want the sub-texel portion

	float2x2 P;

	P[0][0] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(0, 0)).r;
	P[0][1] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(1, 0)).r;
	P[1][0] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(0, 1)).r;
	P[1][1] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(1, 1)).r;

	float dxA = lerp( P[0][0], P[0][1], f.x );
	float dxB = lerp( P[1][0], P[1][1], f.x );
	float height = lerp( dxA, dxB, f.y );

	return (height - g_DisplacementBias) * g_DisplacementScale;
}


float SampleDisplacementMapBilinear2( float2 UV, float LOD )
{
	float3 Dims;
	g_DisplacementMap.GetDimensions( (uint)LOD, Dims.x, Dims.y, Dims.z );

	// Read heights
	float4 Taps = g_DisplacementMap.GatherRed( displacementTapSampler, UV ).wzxy;

	float2 interp = frac( (UV * Dims.xy)-0.5f );	//remove half texel offset
	float dxA = lerp( Taps.x, Taps.y, interp.x );
	float dxB = lerp( Taps.z, Taps.w, interp.x );
	float height = lerp( dxA, dxB, interp.y );

	return (height - g_DisplacementBias) * g_DisplacementScale;
}

float SampleDisplacementMapSmooth( float2 UV )
{
	float3 Dims;
	g_DisplacementMap.GetDimensions( g_DisplacementBlur, Dims.x, Dims.y, Dims.z );

	float2 imageCoordf = UV * Dims.xy;

	uint3 imageCoordi = uint3( imageCoordf.x, imageCoordf.y, 0 );	//half texel offset for sampling

	// Read height
	float Taps[4];
	Taps[0] = g_DisplacementMap.Load( imageCoordi, uint2(0, 0) ).r;
	Taps[1] = g_DisplacementMap.Load( imageCoordi, uint2(1, 0) ).r;
	Taps[2] = g_DisplacementMap.Load( imageCoordi, uint2(0, 1) ).r;
	Taps[3] = g_DisplacementMap.Load( imageCoordi, uint2(1, 1) ).r;

	float2 interp = frac( imageCoordf );
//	float dxA = smoothstep( Taps[0], Taps[1], interp.x );
//	float dxB = smoothstep( Taps[2], Taps[3], interp.x );
//	float height = smoothstep( dxA, dxB, interp.y );

	float height = smoothstep( Taps[0], Taps[1], interp.x );

	return (height - g_DisplacementBias) * g_DisplacementScale;
}

/*
float SampleDisplacementMapBicubic( float2 UV )
{
	float3 Dims;
	g_DisplacementMap.GetDimensions( g_DisplacementBlur, Dims.x, Dims.y, Dims.z );

	float2 imageCoordf = frac(UV) * Dims.xy;

	uint3 imageCoordi = uint3( imageCoordf.x + 0.5f, imageCoordf.GetY + 0.5f, g_DisplacementBlur );	//half texel offset for sampling

	//read center and neighbors
	float Taps[9];
	Taps[0] = g_DisplacementMap.Load( imageCoordi + uint3(-1,-1,0) ).r;
	Taps[1] = g_DisplacementMap.Load( imageCoordi + uint3( 0,-1,0) ).r;
	Taps[2] = g_DisplacementMap.Load( imageCoordi + uint3( 1,-1,0) ).r;
	Taps[3] = g_DisplacementMap.Load( imageCoordi + uint3(-1, 0,0) ).r;
	Taps[4] = g_DisplacementMap.Load( imageCoordi + uint3( 0, 0,0) ).r;
	Taps[5] = g_DisplacementMap.Load( imageCoordi + uint3( 1, 0,0) ).r;
	Taps[6] = g_DisplacementMap.Load( imageCoordi + uint3(-1, 1,0) ).r;
	Taps[7] = g_DisplacementMap.Load( imageCoordi + uint3( 0, 1,0) ).r;
	Taps[8] = g_DisplacementMap.Load( imageCoordi + uint3( 1, 1,0) ).r;


	return (height - g_DisplacementBias) * g_DisplacementScale;
}
*/

/*
float4x4 CubicCoefficientMat =
{
	0, 2, 0, 0,
	-1, 0, 1, 0,
	2,-5, 4,-1,
	-1, 3,-3, 1 
};

float CubicFilter( float xValue, float4 Colors )
{
	float4 Cubics = float4(1,xValue,xValue*xValue,xValue*xValue*xValue) * 0.5f;
	float4 Weights = mul( Cubics, CubicCoefficientMat );

	return dot( Weights, Colors );
}

float DispSampLOD( float2 tex )
{
	return g_DisplacementMap.SampleLevel( displacementTapSampler, tex, g_DisplacementBlur*10.0f).x;	//blur handles 10 mip levels
}

float SampleDisplacementMapBicubic( float2 t )
{
	float4 Colors;
	float4 Rows;

	float3 Dims;
	g_DisplacementMap.GetDimensions( g_DisplacementBlur, Dims.x, Dims.y, Dims.z );

//	Dims.xy /= (floor(g_DisplacementBlur)+1.0f);

	float2 s = t - (0.5 / Dims.xy);

	float2 f = frac(s * Dims.xy);  // we want the sub-texel portion

	float2 O1 = 1.0f / Dims.xy;
	float2 O2 = 2.0f / Dims.xy;

	Colors[0] = DispSampLOD( s + float2(-O1.x, -O1.y));
	Colors[1] = DispSampLOD( s + float2(0, -O1.y));
	Colors[2] = DispSampLOD( s + float2(O1.x, -O1.y));
	Colors[3] = DispSampLOD( s + float2(O2.x, -O1.y));

	Rows[0] = CubicFilter( f.x, Colors );

	Colors[0] = DispSampLOD( s + float2(-O1.x, 0));
	Colors[1] = DispSampLOD( s + float2(0, 0));
	Colors[2] = DispSampLOD( s + float2(O1.x, 0));
	Colors[3] = DispSampLOD( s + float2(O2.x, 0));

	Rows[1] = CubicFilter( f.x, Colors );

	Colors[0] = DispSampLOD( s + float2(-O1.x, O1.y));
	Colors[1] = DispSampLOD( s + float2(0, O1.y));
	Colors[2] = DispSampLOD( s + float2(O1.x, O1.y));
	Colors[3] = DispSampLOD( s + float2(O2.x, O1.y));

	Rows[2] = CubicFilter( f.x, Colors );

	Colors[0] = DispSampLOD( s + float2(-O1.x, O2.y));
	Colors[1] = DispSampLOD( s + float2(0, O2.y));
	Colors[2] = DispSampLOD( s + float2(O1.x, O2.y));
	Colors[3] = DispSampLOD( s + float2(O2.x, O2.y));

	Rows[3] = CubicFilter( f.x, Colors );

	float height = CubicFilter( f.y, Rows );

	return (height - g_DisplacementBias) * g_DisplacementScale;
}
*/

static const float4x4 WeightMat =
{
	0, 2, 0, 0,
	-1, 0, 1, 0, 
	2,-5, 4,-1,
	-1, 3,-3, 1 
};

float MitchellNetravali(float x, float B, float C)
{
	float ax = abs(x);
	if(ax < 1)
	{
		return ((12 - 9 * B - 6 * C) * ax * ax * ax + (-18 + 12 * B + 6 * C) * ax * ax + (6 - 2 * B)) / 6;
	}
	else if((ax >= 1) && (ax < 2))
	{
		return ((-B - 6 * C) * ax * ax * ax + (6 * B + 30 * C) * ax * ax + (-12 * B - 48 * C) * ax + (8 * B + 24 * C)) / 6;
	}
	return 0;
}

float4 BC_spline( float Val, float B, float C )
{
	return float4(
		MitchellNetravali(Val + 1, B, C),
		MitchellNetravali(Val, B, C),
		MitchellNetravali(1 - Val, B, C),
		MitchellNetravali(2 - Val, B, C));
}

float MitchellNetravaliTan(float x, float B, float C)
{
	float ax = abs(x);
	if(ax < 1)
	{
		return -((6*ax*ax-4*ax)*C+(9*ax*ax-8*ax)*B-12*ax*ax+12*ax)/2;
	}
	else if((ax >= 1) && (ax < 2))
	{
		return -((6*ax*ax-20*ax+16)*C+(ax*ax-4*ax+4)*B)/2;
	}
	return 0;
}

float4 BC_splineTan( float Val, float B, float C )
{
	return float4(
		MitchellNetravaliTan(Val + 1, B, C),
		MitchellNetravaliTan(Val, B, C),
		MitchellNetravaliTan(1 - Val, B, C),
		MitchellNetravaliTan(2 - Val, B, C));
}

float cubicFilterBC( float xValue, float4 C )
{
	float4 h = BC_spline( xValue, 0, 0 );
	return dot( C, h );
}

float cubicFilterTanBC( float xValue, float4 C )
{
	float4 h = BC_splineTan( xValue, 0, 0 );
	return dot( C, h );
}

float cubicFilter( float X, float4 C)
{
	float4 t = float4(1,X,X*X,X*X*X) * 0.5f;
	float4 h = mul( t, WeightMat );
	return dot( C, h);
}

float cubicFilterTan( float X, float4 C)
{
	float4 t = float4(0,1,2*X,3*X*X) * 0.5f;
	float4 h = mul( t, WeightMat );
	return dot( C, h);
}

float DisplacementLod( float2 vUV, float LOD )
{
	return g_DisplacementMap.SampleLevel( displacementTapSampler, vUV, LOD-0.5f ).x;
}

float SampleDisplacementBiCubic( float2 UV, float LOD, out float3 Normal )
{
	float2 Dims;
	float Mips;
	g_DisplacementMap.GetDimensions( LOD, Dims.x, Dims.y, Mips );

	float MipLevel = LOD-0.5f;	//remove stupid half texel mip level offset

	//Note this aligns the lower mip levels for central sampling,
	//but causes sample rounding errors
	//without the sampling works correctly but lower mip levels shift
	UV += -0.5f / Dims;

	//remove half texel offset
	float2 f = frac(UV * Dims);  // we want the sub-texel portion

	float4x4 P;
	P[0][0] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(-1, -1)).r;
	P[0][1] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(0, -1)).r;
	P[0][2] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(1, -1)).r;
	P[0][3] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(2, -1)).r;
	P[1][0] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(-1, 0)).r;
	P[1][1] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(0, 0)).r;
	P[1][2] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(1, 0)).r;
	P[1][3] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(2, 0)).r;
	P[2][0] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(-1, 1)).r;
	P[2][1] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(0, 1)).r;
	P[2][2] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(1, 1)).r;
	P[2][3] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(2, 1)).r;
	P[3][0] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(-1, 2)).r;
	P[3][1] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(0, 2)).r;
	P[3][2] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(1, 2)).r;
	P[3][3] = g_DisplacementMap.SampleLevel( displacementTapSampler, UV, MipLevel, int2(2, 2)).r;

	float4 Column;

	Column.x = cubicFilter( f.x, P[0] );
	Column.y = cubicFilter( f.x, P[1] );
	Column.z = cubicFilter( f.x, P[2] );
	Column.w = cubicFilter( f.x, P[3] );
	float height = cubicFilter( f.y, Column );
	float BiTan = cubicFilterTan( f.y, Column );

	Column.x = cubicFilterTan( f.x, P[0] );
	Column.y = cubicFilterTan( f.x, P[1] );
	Column.z = cubicFilterTan( f.x, P[2] );
	Column.w = cubicFilterTan( f.x, P[3] );
	float Tan = cubicFilter( f.y, Column );

//	float UVSize = g_ObjectUVScale.x / g_DisplacementScale;
	float UVSize = abs(g_DisplacementScale*g_ObjectUVScale.x);
//	float UVSize = abs(g_DisplacementScale*10);

	float3 nx = float3( 1, 0, Tan*UVSize );
	float3 ny = float3( 0, 1, BiTan*UVSize ); 
	Normal = normalize(cross(nx, ny));

	return (height + g_DisplacementBias) * g_DisplacementScale;
}

float SampleDisplacementBiCubic2( float2 UV, float LOD, out float3 Normal )
{
	float2 Dims;
	float Mips;
	g_DisplacementMap.GetDimensions( LOD, Dims.x, Dims.y, Mips );

	//remove half texel offset
	float2 f = frac((UV * Dims) - 0.5f );  // we want the sub-texel portion

	/*
		Sample a 4x4 block of pixels using Gather (AKA Fetch4) to grab four 2x2 blocks.
		Gather returns the 4 samples like this, hence the .wzxy swizzle below to get them into a more intuitive ordering.
		+---+---+
		| W | Z |
		+---+---+
		| X | Y |
		+---+---+
	*/
	float4 UL = g_DisplacementMap.GatherRed( displacementTapSampler, UV, int2(-1,-1) ).wzxy;
	float4 UR = g_DisplacementMap.GatherRed( displacementTapSampler, UV, int2( 1,-1) ).wzxy;
	float4 LL = g_DisplacementMap.GatherRed( displacementTapSampler, UV, int2(-1, 1) ).wzxy;
	float4 LR = g_DisplacementMap.GatherRed( displacementTapSampler, UV, int2( 1, 1) ).wzxy;

	float4x4 P =
	{
		float4( UL.xy, UR.xy ),
		float4( UL.zw, UR.zw ),
		float4( LL.xy, LR.xy ),
		float4( LL.zw, LR.zw ),
	};

	float4 Row;
	Row.x = cubicFilter( f.x, P[0] );
	Row.y = cubicFilter( f.x, P[1] );
	Row.z = cubicFilter( f.x, P[2] );
	Row.w = cubicFilter( f.x, P[3] );
	float height = cubicFilter( f.y, Row );
	float BiTan = cubicFilterTan( f.y, Row );

	Row.x = cubicFilterTan( f.x, P[0] );
	Row.y = cubicFilterTan( f.x, P[1] );
	Row.z = cubicFilterTan( f.x, P[2] );
	Row.w = cubicFilterTan( f.x, P[3] );
	float Tan = cubicFilter( f.y, Row );

//	float UVSize = g_ObjectUVScale.x / g_DisplacementScale;
	float UVSize = abs(g_DisplacementScale*g_ObjectUVScale.x);

	float3 nx = float3( 1, 0, Tan*UVSize );
	float3 ny = float3( 0, 1, BiTan*UVSize ); 
	Normal = normalize(cross(nx, ny));

	return (height + g_DisplacementBias) * g_DisplacementScale;
}


float SampleDisplacementTriCubic( float2 UV, float LOD, out float3 Normal )
{
	float3 NA, NB;
	float LOD0 = SampleDisplacementBiCubic( UV, LOD, NA );
	float LOD1 = SampleDisplacementBiCubic( UV, LOD+1, NB );
	Normal = normalize( lerp( NA, NB, frac( LOD )));
	return lerp( LOD0, LOD1, frac( LOD ));
}

//----Will sample the displacement map and translate along the normal.
//----With the exception of floating point textures all samples are scaled to real world
//------value and biased adjust it's center point.  A blur into mip level is also provided.
float3 DisplaceVertex( float3 Pos, float3 Normal, float2 TexCoords )
{
	if( g_hasDisplacementMap )
	{
#ifdef BICUBIC_DISPLACEMENT
		float3 TanNorm;
		float height = SampleDisplacementTriCubic( TexCoords, g_DisplacementBlur*10.0f, TanNorm );
#else
		float height = SampleDisplacementMapBilinear( TexCoords, g_DisplacementBlur*10.0f );
//		float height = SampleDisplacementMap( TexCoords );
#endif
	//	float height = SampleDisplacementMapBilinear( TexCoords );
	//	float height = SampleDisplacementMapSmooth( TexCoords );
		return Pos + (normalize(Normal) * height);
	}
	else return Pos;
}

float3 DisplaceVertexNormal( float3 Pos, float3 Normal, float2 TexCoords, out float3 TanNorm )
{
	TanNorm = float3( 0,0,1 );
	if( g_hasDisplacementMap )
	{
#ifdef BICUBIC_DISPLACEMENT
//		float height = SampleDisplacementBiCubic( TexCoords, g_DisplacementBlur*10.0f, TanNorm );
		float height = SampleDisplacementTriCubic( TexCoords, g_DisplacementBlur*10.0f, TanNorm );
#else
		float height = SampleDisplacementMapBilinear( TexCoords, g_DisplacementBlur*10.0f );
//		float height = SampleDisplacementMap( TexCoords );
#endif
		return Pos + (normalize(Normal) * height);
	}
	else return Pos;
}

/*********** data ******/
// hasHardwareTessellation is in MaterialCommon (Globals.hlsli).

/*
bool g_bEnableTessellation
<
	string SasUiControl = "CheckBox";
	string SasUiLabel = "Enable";
	string SasUiDescription = "Use ATI Hardware Tessellation.";
	string UiCategory = "Tessellation";
> = false;

float g_TessellationValue : TessellationValue
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Amount";
	string SasUiDescription = "Amount of Tessellation.";
	string UiCategory = "Tessellation";
	float SasUiMin = 1.0;
	float SasUiMax = 15.0;
> = 0.0f;
*/
//bool g_bFlatTessellate
//<
//	string SasUiControl = "CheckBox";
//	string SasUiLabel = "Flat Tessellate";
//	string SasUiDescription = "To disable PN Smoothing.";
//	string UiCategory = "Tessellation";
//> = true;

//doesn't work
/*
int g_normalMethod
<
string SasUiControl = "ListPicker";
string SasUiLabel = "Normal Method";
string SasUiDescription = "Select how the normals are calculated.";
string UiCategory = "Tessellation";
string SasUiEnum = "Quadratic, Linear, Average, True";
> = 0;
*/
/*
int g_normalMethod
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Normal Method";
	string SasUiDescription = "Select how the normals are calculated.";
	string UiCategory = "Tessellation";
	float SasUiMin = 0.0;
	float SasUiMax = 3.0;
	float SasUiSteps = 3.0;
> = 0;
*/

/*********** functions ******/

//--------------------------------------------------------------------------------------
// Modulo function
// There is a bug in the % operator in the DirectX SDK compiler
// Below is a working modulo function to replace it.
//--------------------------------------------------------------------------------------

int modulo(int nValue, int nModulo)
{
	return ( nValue - ( (nValue/nModulo)*nModulo ) );
}
/*
float4 SampleVertexData( float index )
{
	return tex2Dlod(meshDataSampler, float4( (modulo(index, MESHDATATEXTURESIZE))/(MESHDATATEXTURESIZE-1.0f), (index/MESHDATATEXTURESIZE)/MESHDATATEXTURESIZE, 0, 0));
}
*/
//--------------------------------------------------------------------------------------
// Calculate parametric coordinates between 0..1 for patch evalution	
//
// Because the tessellation HW implementation uses parametric coordinates between 0 and 0.5 (instead of 0 to 1)
// the order of the superprimitives is switched when parametric coordinates cross the patch's centre.
// For this reason we *must* recalculate our own patch parametric coordinates UVs using cartesian interpolation
// before being able to use them in patch evaluation.

// The patch UVs are defined as such for the four vertices making up the superprimitive:
//
// (0,1)       (1,1)
//      V3---V2
//      |    |
//      |    |
//      V0---V1
// (0,0)       (1,0)
//
// vInputWeights is the vertex input declared as BLENDWEIGHT0 (the input coordinates from the HW tessellation unit)
//--------------------------------------------------------------------------------------
float4 CalculatePatchParametricCoordinates(float4 vInputWeights, uint4 vIndices)
{
	float4 vParametricCoordinates;
	const float2 f2_table[4] = { float2(0,0), float2(1,0), float2(1,1), float2(0,1) };

	// Patch vertex index is the index number modulo 4 i.e. 0, 1, 2 or 3.
	int4 vIndicesModulo4 = vIndices % 4;

	// Base weights to interpolate depend on index value (hence use of a small lookup table)
	float2 BaseWeight_V3 = f2_table[vIndicesModulo4.x];
	float2 BaseWeight_V0 = f2_table[vIndicesModulo4.y];
	float2 BaseWeight_V1 = f2_table[vIndicesModulo4.z];
	float2 BaseWeight_V2 = f2_table[vIndicesModulo4.w];

	// Use Cartesian interpolation to calculate our parametric coordinates for patch evaluation
	float2 UV1 = (BaseWeight_V0 * vInputWeights.x + BaseWeight_V1 * vInputWeights.z);
	float2 UV2 = (BaseWeight_V3 * vInputWeights.x + BaseWeight_V2 * vInputWeights.z);
	vParametricCoordinates.xy =	UV1 * vInputWeights.y + UV2 * vInputWeights.w;

	// Convenience variables: z and w contain (1-u) and (1-v) respectively
	vParametricCoordinates.zw = 1.0 - vParametricCoordinates.xy;

	return vParametricCoordinates;
}


//---------------Linear Interpolation----------------
float4 GetLinearTriangleVertex( VS_INPUT_TESS In )
{
	//trilinear interpolate
	return In.Position0 * In.vBarycentric.x + In.Position1 * In.vBarycentric.y + In.Position2 * In.vBarycentric.z;
}

float3 GetLinearTriangleNormal( VS_INPUT_TESS In )
{
	//trilinear interpolate
	return normalize(In.Normal0 * In.vBarycentric.x + In.Normal1 * In.vBarycentric.y + In.Normal2 * In.vBarycentric.z);
}

float3 GetLinearTriangleTangent( VS_INPUT_TESS In )
{
	//trilinear interpolate
	return normalize(In.Tangent0 * In.vBarycentric.x + In.Tangent1 * In.vBarycentric.y + In.Tangent2 * In.vBarycentric.z);
}

float3 GetLinearTriangleBitangent( VS_INPUT_TESS In )
{
	//trilinear interpolate
	return normalize(In.Binormal0 * In.vBarycentric.x + In.Binormal1 * In.vBarycentric.y + In.Binormal2 * In.vBarycentric.z);
}

float2 GetLinearTriangleTexture( VS_INPUT_TESS In )
{
	//trilinear interpolate
	return In.UV0 * In.vBarycentric.x + In.UV1 * In.vBarycentric.y + In.UV2 * In.vBarycentric.z;
}

float4 PositionTess( VS_INPUT_TESS In, float3 barycenter )
{
	return In.Position0 * barycenter.x + In.Position1 * barycenter.y + In.Position2 * barycenter.z;
}

float3 NormalTess( VS_INPUT_TESS In, float3 barycenter )
{
	return In.Normal0 * barycenter.x + In.Normal1 * barycenter.y + In.Normal2 * barycenter.z;
}

float4 BernsteinPoly( float x, float ix )
{
	return float4( ix*ix*ix, 3*x*ix*ix, 3*x*x*ix, x*x*x );
}

float4 BernsteinPolyD( float x, float ix )
{
	return float4( -3*x*x + 6*x -3, 9*x*x - 12*x + 3, -9*x*x + 6*x, 3*x*x );
}

float3 CalcCubicBezierPatch( in float3 CP[16], in float4 UBasis, in float4 VBasis )
{
	float3 V = float3(0,0,0);
	V += VBasis.x * (CP[0]  * UBasis.x + CP[1]  * UBasis.y + CP[2]  * UBasis.z + CP[3]  * UBasis.w);
	V += VBasis.y * (CP[4]  * UBasis.x + CP[5]  * UBasis.y + CP[6]  * UBasis.z + CP[7]  * UBasis.w);
	V += VBasis.z * (CP[8]  * UBasis.x + CP[9]  * UBasis.y + CP[10] * UBasis.z + CP[11] * UBasis.w);
	V += VBasis.w * (CP[12] * UBasis.x + CP[13] * UBasis.y + CP[14] * UBasis.z + CP[15] * UBasis.w);
	return V;
}

//--------------------------------------------------------------------------------------
// Helper function
//--------------------------------------------------------------------------------------
void BezierRaise( inout float3 pQ[3], out float3 pC[4])
{
	pC[0] = pQ[0];
	pC[3] = pQ[2];

	for( int i=1; i<3; i++ ) 
	{
		pC[i] = ( 1.0f / 3.0f ) * ( pQ[i - 1] * i + ( 3.0f - i ) * pQ[i] );
	}
}

void BezierRaise2( in float3 pQ[3], out float3 pC[2])
{
	for( int i=1; i<3; i++ ) 
	{
		pC[i-1] = ( 1.0f / 3.0f ) * ( pQ[i - 1] * i + ( 3.0f - i ) * pQ[i] );
	}
}


//--------------------------------------------------------------------------------------
// Computes the tangent patch from the input bezier patch
//--------------------------------------------------------------------------------------
void ComputeTanPatch(in float3 vIn[16], inout float3 vOut[16], in float4 fCWts, in float3 vCorner[4], in float3 vCornerLocal[4], in const uint cX, in const uint cY)
{
	float3 vQuad[3];
//	float3 vQuadB[3];
//	float3 vCubic[4];
	float3 vCubic2[2];

	// boundary edges are really simple...
	vQuad[0] = vCornerLocal[0];
	vQuad[2] = vCornerLocal[1];
	vQuad[1] = 3.0f*(vIn[2*cX+0*cY]-vIn[1*cX+0*cY]);

	BezierRaise2(vQuad,vCubic2);
	vOut[1*cX + 0*cY] = vCubic2[0];
	vOut[2*cX + 0*cY] = vCubic2[1];

	vQuad[0] = vCornerLocal[2];
	vQuad[2] = vCornerLocal[3];
	vQuad[1] = 3.0f*(vIn[2*cX+3*cY]-vIn[1*cX+3*cY]);

	BezierRaise2(vQuad,vCubic2);
	vOut[1*cX + 3*cY] = vCubic2[0];
	vOut[2*cX + 3*cY] = vCubic2[1];
}

//---------------Cubic Interpolation----------------
float3 GetPNTriangleVertex( VS_INPUT_TESS In )
{
	float3 Vert = float3(0,0,0);

	// Tricubic Bezier Triangle Patch
	//               3
	//   B(u,v,w) = SUM( Bijk * u^i * v^j * w^k * (3!/(i!*j!*k!)) )
	//             i=j=k=0
	//   w=1-u-v

	//Parametric terms
	float3 I = In.vBarycentric;
	float3 I2 = I * I;
	float3 I3 = I * I2;

	//coordinates
	float3 P1 = In.Position0.xyz;
	float3 P2 = In.Position1.xyz;
	float3 P3 = In.Position2.xyz;
	float3 N1 = In.Normal0;
	float3 N2 = In.Normal1;
	float3 N3 = In.Normal2;

	//tangent scalars
	float s12 = dot(P2 - P1, N1);
	float s21 = dot(P1 - P2, N2);
	float s23 = dot(P3 - P2, N2);
	float s32 = dot(P2 - P3, N3);
	float s31 = dot(P1 - P3, N3);
	float s13 = dot(P3 - P1, N1);

	//cubic bezier cooeficients
	float3 b210 = (2 * P1 + P2 - s12 * N1);      //Tangent Coefficients
	float3 b120 = (2 * P2 + P1 - s21 * N2);
	float3 b021 = (2 * P2 + P3 - s23 * N2);
	float3 b012 = (2 * P3 + P2 - s32 * N3);
	float3 b102 = (2 * P3 + P1 - s31 * N3);
	float3 b201 = (2 * P1 + P3 - s13 * N1);

	float3 b111 = ((b210 + b120 + b021 + b012 + b102 + b201)/2) - (P1 + P2 + P3);   //Center Coefficient

	//displacement field - 10D vector product (less instructions but uglier)
	Vert = (I3.x) * P1 + (I3.y) * P2 + (I3.z) * P3 +
		(I.x*I2.z) * b102 + (I.y*I2.x) * b210 + (I.z*I2.y) * b021 +
		(I.x*I2.y) * b120 + (I.y*I2.z) * b012 + (I.z*I2.x) * b201 +
		(I.x*I.y*I.z) * b111;   

	return Vert;
}

float3 GetPNTriangleTangent( VS_INPUT_TESS In )
{
	float3 Tan = float3(0,0,0);

	// Tricubic Bezier Triangle Patch
	//
	//   dB(x,y,1-x-y) / dx

	//Parametric terms
	float x = In.vBarycentric.x;
	float x2 = x*x;
	float y = In.vBarycentric.y;
	float y2 = y*y;

	//coordinates
	float3 P1 = In.Position0.xyz;
	float3 P2 = In.Position1.xyz;
	float3 P3 = In.Position2.xyz;
	float3 N1 = In.Normal0;
	float3 N2 = In.Normal1;
	float3 N3 = In.Normal2;

	//tangent scalars
	float s12 = dot(P2 - P1, N1);
	float s21 = dot(P1 - P2, N2);
	float s23 = dot(P3 - P2, N2);
	float s32 = dot(P2 - P3, N3);
	float s31 = dot(P1 - P3, N3);
	float s13 = dot(P3 - P1, N1);

	//cubic bezier cooeficients
	float3 b300 = P1;
	float3 b030 = P2;
	float3 b003 = P3;
	float3 b210 = (2 * P1 + P2 - s12 * N1)/3;      //Tangent Coefficients
	float3 b120 = (2 * P2 + P1 - s21 * N2)/3;
	float3 b021 = (2 * P2 + P3 - s23 * N2)/3;
	float3 b012 = (2 * P3 + P2 - s32 * N3)/3;
	float3 b102 = (2 * P3 + P1 - s31 * N3)/3;
	float3 b201 = (2 * P1 + P3 - s13 * N1)/3;

	float3 b111 = ((b210 + b120 + b021 + b012 + b102 + b201)/4) - ((P1 + P2 + P3)/6);   //Center Coefficient

	//Tangent field
	///25D vector product (less instructions but uglier)
	Tan = -3*b021*y2 - 3*b003 + 3*b300*x2 - 9*b201*x2 + 3*b102 + 12*b102*x*y + 6*b003*y + 6*b003*x + 6*b012*y2 - 3*b003*y2 + 3*b120*y2 + 6*b210*x*y - 3*b003*x2 - 12*b111*x*y + 9*b102*x2 - 6*b102*y - 12*b102*x - 6*b111*y2 + 3*b102*y2 - 6*b201*x*y + 6*b111*y + 6*b201*x + 6*b012*y*x - 6*b012*y - 6*b003*x*y;

	return Tan;
}

float3 GetPNTriangleBitangent( VS_INPUT_TESS In )
{
	float3 BiTan = float3(0,0,0);

	// Tricubic Bezier Triangle Patch
	//
	//   dB(x,y,1-x-y) / dy

	//Parametric terms
	float x = In.vBarycentric.x;
	float x2 = x*x;
	float y = In.vBarycentric.y;
	float y2 = y*y;

	//coordinates
	float3 P1 = In.Position0.xyz;
	float3 P2 = In.Position1.xyz;
	float3 P3 = In.Position2.xyz;
	float3 N1 = In.Normal0;
	float3 N2 = In.Normal1;
	float3 N3 = In.Normal2;

	//tangent scalars
	float s12 = dot(P2 - P1, N1);
	float s21 = dot(P1 - P2, N2);
	float s23 = dot(P3 - P2, N2);
	float s32 = dot(P2 - P3, N3);
	float s31 = dot(P1 - P3, N3);
	float s13 = dot(P3 - P1, N1);

	//cubic bezier cooeficients
	float3 b300 = P1;
	float3 b030 = P2;
	float3 b003 = P3;
	float3 b210 = (2 * P1 + P2 - s12 * N1)/3;      //Tangent Coefficients
	float3 b120 = (2 * P2 + P1 - s21 * N2)/3;
	float3 b021 = (2 * P2 + P3 - s23 * N2)/3;
	float3 b012 = (2 * P3 + P2 - s32 * N3)/3;
	float3 b102 = (2 * P3 + P1 - s31 * N3)/3;
	float3 b201 = (2 * P1 + P3 - s13 * N1)/3;

	float3 b111 = ((b210 + b120 + b021 + b012 + b102 + b201)/4) - ((P1 + P2 + P3)/6);   //Center Coefficient

	//BiTangent field
	///25D vector product (less instructions but uglier)
	BiTan = -12*b111*x*y - 6*b003*x*y + 3*b210*x2 + 6*b111*x + 3*b012 - 6*b111*x2 - 3*b003 + 6*b102*x2 - 6*b012*x + 6*b003*x + 12*b012*x*y - 9*b012*y2 - 3*b201*x2 + 9*b012*y2 + 6*b120*x*y + 3*b030*y2 - 12*b012*y + 3*b012*x2 + 6*b003*y - 3*b003*x2 - 3*b003*y2 + 6*b102*x*y + 6*b021*y - 6*b021*y*x - 6*b102*x;

	return BiTan;
}

//-------------Quadratic Interpolation
float3 GetPNTriangleNormal( VS_INPUT_TESS In )
{
	float3 Norm = float3(0,0,0);

	// triquadratic Bezier Triangle Patch
	//               2
	//   N(u,v,w) = SUM( Vijk * u^i * v^j * w^k )
	//            i=j=k=0
	//   w=1-u-v

	//Parametric terms
	float3 I = In.vBarycentric;
	float3 I2 = I * I;

	//coordinates
	float3 P1 = In.Position0.xyz;
	float3 P2 = In.Position1.xyz;
	float3 P3 = In.Position2.xyz;
	float3 N1 = In.Normal0;
	float3 N2 = In.Normal1;
	float3 N3 = In.Normal2;

	//edge scalars
	float v12 = (dot(P2-P1,N2+N1) / dot(P2-P1,P2-P1)) * 4;
	float v23 = (dot(P3-P2,N3+N2) / dot(P3-P2,P3-P2)) * 4;
	float v31 = (dot(P1-P3,N1+N3) / dot(P1-P3,P1-P3)) * 4;

	//quadratic bezier cooeficients
	float3 n110 = (N1 + N2 - v12 * (P2 - P1));
	float3 n011 = (N2 + N3 - v23 * (P3 - P2));
	float3 n101 = (N3 + N1 - v31 * (P1 - P3));

	//displacement field
	Norm = N1 * I2.x + N2 * I2.y + N3 * I2.z +
		n110 * I.x * I.y + n011 * I.y * I.z + n101 * I.x * I.z;

	return normalize(Norm);
}

//---------Other Normal calculation methods
float3 GetTrueTriangleNormal( VS_INPUT_TESS In )
{
	float3 Norm = float3(0,0,0);

	float3 Tan = GetPNTriangleTangent( In );
	float3 BiTan = GetPNTriangleBitangent( In );

	Norm = normalize( cross( Tan, BiTan ));

	//Get Interpolated Normal
	float3 N = GetLinearTriangleNormal( In );
	Norm *= sign(dot(N,Norm));   //flip if opposite of interpolated normal

	return Norm;
}

float3 GetAverageTriangleNormal( VS_INPUT_TESS In, float3 Center )
{
	float3 Norm = float3(0,0,0);

	//coordinates
	float3 P1 = In.Position0.xyz;
	float3 P2 = In.Position1.xyz;
	float3 P3 = In.Position2.xyz;

	float3 V1 = P2 - P1;
	float3 V2 = P3 - P2;
	float3 V3 = P1 - P3;

	Norm = cross( V2, V3 );

	float3 E1 = Center - P1;
	float3 E2 = Center - P2;
	float3 E3 = Center - P3;

	//   Norm += cross( E2, E1 );
	//   Norm += cross( E3, E2 );
	//   Norm += cross( E1, E3 );

	float3 V = GetLinearTriangleVertex( In ).xyz;
	float3 N = GetLinearTriangleNormal( In );

	float3 DV = Center - V;

	//   Norm = N + (V - Center);
	Norm = N + (DV * sign(dot(DV,N)));

	return normalize( Norm );
}

float2 SobelFilter( Texture2D HeightMap, float4 texCoord, float2 TextureSize )
{
	float2 off = 1.0 / TextureSize;
	float lod = texCoord.w + g_DisplacementBlur*10.0f;

	// Take all neighbor samples
	float s00 = HeightMap.SampleLevel(displacementSampler, texCoord.xy + float2(-off.x, -off.y), lod).r;
	float s01 = HeightMap.SampleLevel(displacementSampler, texCoord.xy + float2( 0,   -off.y), lod).r;
	float s02 = HeightMap.SampleLevel(displacementSampler, texCoord.xy + float2( off.x, -off.y), lod).r;

	float s10 = HeightMap.SampleLevel(displacementSampler, texCoord.xy + float2(-off.x,  0), lod).r;
	float s12 = HeightMap.SampleLevel(displacementSampler, texCoord.xy + float2( off.x,  0), lod).r;

	float s20 = HeightMap.SampleLevel(displacementSampler, texCoord.xy + float2(-off.x,  off.y), lod).r;
	float s21 = HeightMap.SampleLevel(displacementSampler, texCoord.xy + float2( 0,    off.y), lod).r;
	float s22 = HeightMap.SampleLevel(displacementSampler, texCoord.xy + float2( off.x,  off.y), lod).r;

	// Slope in X direction
	float sobelX = s00 + 2 * s10 + s20 - s02 - 2 * s12 - s22;
	// Slope in Y direction
	float sobelY = s00 + 2 * s01 + s02 - s20 - 2 * s21 - s22;

	return float2( sobelX, sobelY );
}

float3 ComputeNormalFromHeightMap(float2 i_uv, Texture2D i_heightMap, float i_lod,
								  float i_normalStrength)
{
	float2 texelSize;
	float mips;
	i_heightMap.GetDimensions( i_lod, texelSize.x, texelSize.y, mips );

	// texel size
	texelSize = 1/texelSize;
/*
	float tl = abs(i_heightMap.SampleLevel(displacementSampler, i_uv + texelSize * float2(-1, -1), i_lod).x);   // top left
	float  l = abs(i_heightMap.SampleLevel(displacementSampler, i_uv + texelSize * float2(-1,  0), i_lod).x);   // left
	float bl = abs(i_heightMap.SampleLevel(displacementSampler, i_uv + texelSize * float2(-1,  1), i_lod).x);   // bottom left
	float  t = abs(i_heightMap.SampleLevel(displacementSampler, i_uv + texelSize * float2( 0, -1), i_lod).x);   // top
	float  b = abs(i_heightMap.SampleLevel(displacementSampler, i_uv + texelSize * float2( 0,  1), i_lod).x);   // bottom
	float tr = abs(i_heightMap.SampleLevel(displacementSampler, i_uv + texelSize * float2( 1, -1), i_lod).x);   // top right
	float  r = abs(i_heightMap.SampleLevel(displacementSampler, i_uv + texelSize * float2( 1,  0), i_lod).x);   // right
	float br = abs(i_heightMap.SampleLevel(displacementSampler, i_uv + texelSize * float2( 1,  1), i_lod).x);   // bottom right
*/ 
	float tl = abs(i_heightMap.SampleLevel(displacementSampler, i_uv, i_lod, int2(-1, -1)).x);   // top left
	float  l = abs(i_heightMap.SampleLevel(displacementSampler, i_uv, i_lod, int2(-1,  0)).x);   // left
	float bl = abs(i_heightMap.SampleLevel(displacementSampler, i_uv, i_lod, int2(-1,  1)).x);   // bottom left
	float  t = abs(i_heightMap.SampleLevel(displacementSampler, i_uv, i_lod, int2( 0, -1)).x);   // top
	float  b = abs(i_heightMap.SampleLevel(displacementSampler, i_uv, i_lod, int2( 0,  1)).x);   // bottom
	float tr = abs(i_heightMap.SampleLevel(displacementSampler, i_uv, i_lod, int2( 1, -1)).x);   // top right
	float  r = abs(i_heightMap.SampleLevel(displacementSampler, i_uv, i_lod, int2( 1,  0)).x);   // right
	float br = abs(i_heightMap.SampleLevel(displacementSampler, i_uv, i_lod, int2( 1,  1)).x);   // bottom right
 
	// Compute dx using Sobel:
	//           -1 0 1 
	//           -2 0 2
	//           -1 0 1
	float dX = tr + 2*r + br -tl - 2*l - bl;

	// Compute dy using Sobel:
	//           -1 -2 -1 
	//            0  0  0
	//            1  2  1
	float dY = bl + 2*b + br -tl - 2*t - tr;

	// Build the normalized normal
	float3 N = normalize(float3(dX, dY, 1.0f / i_normalStrength));

	//convert (-1.0 , 1.0) to (0.0 , 1.0), if needed
	return N;// * 0.5f + 0.5f;
}

// compute the 3x3 tranform from world space to tangent space,
// transforming basis vectors to world space
// also send transpose of this matrix to the pixel shader
// so that it can transform the normal into world space to 
// compute the reflection vector for env mapping
void TangentToWorldSpace(uniform float3x3 objToWorld,
							in float3 objTangent,
							in float3 objBinormal,
							in float3 objNormal,
							out float3 tanToWorldX,
							out float3 tanToWorldY,
							out float3 tanToWorldZ)
{
	float3 wTangent = mul(objToWorld, objTangent).xyz;
	float3 wBinormal = mul(objToWorld, objBinormal).xyz;
	float3 wNormal = mul(objToWorld, objNormal).xyz;

	tanToWorldX = float3(wTangent.x, wBinormal.x, wNormal.x);
	tanToWorldY = float3(wTangent.y, wBinormal.y, wNormal.y);
	tanToWorldZ = float3(wTangent.z, wBinormal.z, wNormal.z);
}

//Uses central differencing
//returns tangent space normal
float3 NormalFromDisplacement( float2 texCoord, float2 UVScale )
{
	float2 off = 1.0f / g_DisplacementMapSize;

	float2 distance = UVScale * off;

	float ctr = SampleDisplacementMap( texCoord + float2(0, 0) );
	float ul = SampleDisplacementMap( texCoord + float2(-off.x,  0));
	float ur = SampleDisplacementMap( texCoord + float2( off.x,  0));
	float vt = SampleDisplacementMap( texCoord + float2( 0, -off.x));
	float vb = SampleDisplacementMap( texCoord + float2( 0,  off.x));

	// Average normals four neighboring quads:
	// 
	//       V2
	//   U2  X   U	
	//       V	

	float3 vUX  = float3(  distance.x, 0, ur  - ctr ) ;
	float3 vVX  = float3(           0, -distance.y, vt  - ctr ) ;
	float3 vU2X = float3( -distance.x, 0, ul - ctr ) ;
	float3 vV2X = float3(           0, distance.y, vb - ctr ) ;

	return -normalize( normalize( cross(  vUX,  vVX ) ) +  
		normalize( cross(  vVX, vU2X ) ) +
		normalize( cross( vU2X, vV2X ) ) +
		normalize( cross( vV2X,  vUX ) ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
float3x3 MakeFromToRotationMatrixFast ( float3 vFrom, float3 vTo )
{
	float3x3 mResult;

	float3 axis = cross( vFrom, vTo );		//axis of rotation
	float  ang = dot( vFrom, vTo );			//angle of rotation
	float  fH = 1.0 / (1.0 + ang);

	mResult[0][0] = ang + fH   * axis.x * axis.x;
	mResult[1][0] = fH * axis.x * axis.y + axis.z;
	mResult[2][0] = fH * axis.x * axis.z - axis.y;

	mResult[0][1] = fH * axis.x * axis.y - axis.z;
	mResult[1][1] = ang + fH   * axis.y * axis.y;
	mResult[2][1] = fH * axis.y * axis.z + axis.x;

	mResult[0][2] = fH * axis.x * axis.z + axis.y;
	mResult[1][2] = fH * axis.y * axis.z - axis.x;
	mResult[2][2] = ang + fH   * axis.z * axis.z;

	return mResult;

}  // End of MakeFromToRotationMatrixFast(..)


float3 BarycentricFromUV( VS_INPUT_TESS In, float2 UV )
{
	float3 Barycenter;

	float det = ((In.UV0.x * In.UV1.y) - (In.UV0.x * In.UV2.y) - (In.UV1.x * In.UV0.y) + (In.UV1.x * In.UV2.y) + (In.UV2.x * In.UV0.y) - (In.UV2.x * In.UV1.y));

	Barycenter.x = ((UV.x * In.UV1.y) - (UV.x * In.UV2.y) - (In.UV1.x * UV.y) + (In.UV1.x * In.UV2.y) + (In.UV2.x * UV.y) - (In.UV2.x * In.UV1.y)) / det;
	Barycenter.y = ((In.UV0.x * UV.y) - (In.UV0.x * In.UV2.y) - (UV.x * In.UV0.y) + (UV.x * In.UV2.y) + (In.UV2.x * In.UV0.y) - (In.UV2.x * UV.y)) / det;
	Barycenter.z = 1 - Barycenter.x - Barycenter.y;

	return Barycenter;
}

float3 TangentNormaltoWorld( in TANGENT_MATRIX mat, in float3 TanNormal, bool FrontFace : SV_ISFRONTFACE )
{
	float3x3 TanMat = {mat.X, mat.Y, mat.Z};
	// Convert normal to world space
	float3 worldNormal = mul( TanNormal, TanMat );

	if( g_bDoubleSided && !(FrontFace ^ g_IsReflectionGen))
	{
		worldNormal = -worldNormal;
	}

	return normalize(worldNormal);
}

float3 GetNormal( in TANGENT_VERTEX V, in bool FrontFace : SV_ISFRONTFACE )
{
	float3 worldNormal = V.WorldTan.Z;

	if( g_bDoubleSided && !(FrontFace ^ g_IsReflectionGen))
	{
		worldNormal = -worldNormal;
	}

	return normalize(worldNormal);
}

float3 GetBumpNormal( in TANGENT_VERTEX V, in bool hasBump, in Texture2D bumpMap, in float bumpMapScale, in bool FrontFace : SV_ISFRONTFACE )
{
	if( hasBump )
	{
		float3 TanNormal = ((bumpMap.Sample( bumpMapSampler, V.TexCoord0.xy).xyz * 2) - 1) * float3(bumpMapScale,bumpMapScale,1);
		return TangentNormaltoWorld( V.WorldTan, TanNormal, FrontFace );
	}
	else
	{
		return GetNormal( V, FrontFace );
	}
}

float3 Get2BumpNormal( in TANGENT_VERTEX V,
					  in bool hasBump, in Texture2D bumpMap, in float bumpMapScale,
					  in bool hasMicroBump, in Texture2D microBumpMap, in float microBumpMapScale,
					  in bool FrontFace : SV_ISFRONTFACE )
{
	if( hasMicroBump || hasBump )
	{
		float3 bumpNormal = float3(0,0,1);
		if( hasMicroBump )
		{
			bumpNormal += (microBumpMap.Sample( bumpMapSampler, V.TexCoord0.xy*microBumpMapScale).xyz * 2) - 1;
		}
		if( hasBump )
		{
			bumpNormal += ((bumpMap.Sample( bumpMapSampler, V.TexCoord0.xy).xyz * 2) - 1) * float3(bumpMapScale,bumpMapScale,1);
		}
		return TangentNormaltoWorld( V.WorldTan, bumpNormal, FrontFace );
	}
	else return GetNormal( V, FrontFace );
}

//-----------------------------------------------------
// Alters the tangent basis matrix from a displacement map
// (Pixel Shader only and Tessellated only)
//-----------------------------------------------------

void TangentDisplace( inout TANGENT_VERTEX V )
{
#ifndef BICUBIC_DISPLACEMENT
	if( g_hasDisplacementMap )
	{
//		float2 dP = float2(length(ddx(V.WorldPos)), length(ddy(V.WorldPos)));
//		float dPdU = dP.x / length(ddx(V.TexCoord0));
//		float dPdV = dP.y / length(ddy(V.TexCoord0));
//		float dPdU = length(float2(dP.x/ddx(V.TexCoord0.x), dP.y/ddy(V.TexCoord0.x)));
//		float dPdV = length(float2(dP.x/ddx(V.TexCoord0.y), dP.y/ddy(V.TexCoord0.y)));

//		float3 TanNormal = NormalFromDisplacement( V.TexCoord0, float2( 10, 10 ));
//		float3 TanNormal = NormalFromDisplacement( V.TexCoord0, float2( dPdU, dPdV ));
		float3 TanNormal = NormalFromDisplacement( V.TexCoord0, g_ObjectUVScale );

		float3x3 TanMat = {V.WorldTan.X, V.WorldTan.Y, V.WorldTan.Z};

		// Rotate tangent frame to line up to the bumped normal (for subsequent displacements or normal-mapping)
		float3x3 mBumpRotation = MakeFromToRotationMatrixFast( normalize(V.WorldTan.Z), TanNormal );

		// Convert normal to world space
		V.WorldTan.Z = mul( TanNormal, TanMat );
		//transform tangent frame
		V.WorldTan.X = mul( mBumpRotation, V.WorldTan.X );
		V.WorldTan.Y = mul( mBumpRotation, V.WorldTan.Y );
	}
#endif
}

void AlignTangentSpaceToNormal( inout TANGENT_MATRIX WorldTan, in float3 TanNormal )
{
	WorldTan.Z = TanNormal;
	WorldTan.X = cross( WorldTan.Z, WorldTan.Y );
	WorldTan.Y = cross( WorldTan.X, WorldTan.Z );
}

//--------------------------------------------------------------------------------------
// Returns the screen space position from the given world space patch control point
//--------------------------------------------------------------------------------------
float2 GetScreenSpacePosition   ( 
                                float3 f3Position,              // World space position of patch control point
                                float4x4 f4x4ViewProjection,    // View x Projection matrix
                                float2 fScreenRes             // Screen resolution
                                )
{
    float4 f4ProjectedPosition = mul(f4x4ViewProjection, float4( f3Position, 1.0f ) );
		//mul( float4( f3Position, 1.0f ), f4x4ViewProjection );
    
	// screen pos in -1..1
    float2 f2ScreenPosition = f4ProjectedPosition.xy / f4ProjectedPosition.ww;
    
	// transform -1..1 into 0..(x,y)
    f2ScreenPosition = ( f2ScreenPosition + 1.0f ) * 0.5f * fScreenRes ;

    return f2ScreenPosition;
}
//--------------------------------------------------------------------------------------
// Returns the screen space adaptive tessellation scale factor (0.0f -> 1.0f)
//--------------------------------------------------------------------------------------
float GetScreenSpaceAdaptiveScaleFactor (
                                        float2 f2EdgeScreenPosition0,   // Screen coordinate of the first patch edge control point
                                        float2 f2EdgeScreenPosition1,   // Screen coordinate of the second patch edge control point    
                                        float fMaxEdgeTessFactor,       // Maximum edge tessellation factor                            
                                        float fTargetEdgePrimitiveSize  // Desired primitive edge size in pixels
                                        )
{
    float fEdgeScreenLength = distance( f2EdgeScreenPosition0, f2EdgeScreenPosition1 );

    float fTargetTessFactor = fEdgeScreenLength / fTargetEdgePrimitiveSize;

    fTargetTessFactor /= fMaxEdgeTessFactor;
    
    float fScale = saturate( fTargetTessFactor );
    
    return fScale;
}
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
float GetScreenSpaceAdaptiveTessFactor (
                                        float2 i_f2EdgeScreenPosition0,   // Screen coordinate of the first patch edge control point
                                        float2 i_f2EdgeScreenPosition1,   // Screen coordinate of the second patch edge control point    
                                        float i_fUserEdgeTessFactor,       // Maximum edge tessellation factor                            
                                        float i_fLimitEdgePrimitiveSize  // Min primitive edge size in pixels
                                        )
{
    float fEdgeScreenLength = distance( i_f2EdgeScreenPosition0, i_f2EdgeScreenPosition1 );

	// how many times do i need to divide this edge to get to the size limit?
    float fTargetTessFactor = fEdgeScreenLength / i_fLimitEdgePrimitiveSize;
	// conservative: round up to an integer number of divisions
	//fTargetTessFactor = ceil(fTargetTessFactor);

	// don't tessellate beyond the target limit.
	return clamp(i_fUserEdgeTessFactor, 1.0f, max(1.0f, fTargetTessFactor));
}


//--------------------------------------------------------------------------------------
// Hull shader
//--------------------------------------------------------------------------------------

struct HS_CONSTANT_DATA_OUTPUT
{
	float    Edges[3]         : SV_TessFactor;
	float    Inside           : SV_InsideTessFactor;
};

//--------------------------------------------------------------------------------------
// this is the true adaptive routine for triangles. verts must come in world space.
//--------------------------------------------------------------------------------------
HS_CONSTANT_DATA_OUTPUT AdaptiveTessellate(float3 i_V0, float3 i_V1, float3 i_V2)
{
	HS_CONSTANT_DATA_OUTPUT output = (HS_CONSTANT_DATA_OUTPUT)0;

//	float3 N = cross(i_V0-i_V2, i_V1-i_V0);
//	if (dot(N, g_eyePos.xyz) < 0)
//	{
//		output.Edges[0] = 0;
//		output.Edges[1] = 0;
//		output.Edges[2] = 0;
//		output.Inside   = 0;
//	}
//	else
	{
		float4 vEdgeTessellationFactors;
		// Tessellation level fixed by variable
		vEdgeTessellationFactors = g_vTessellationFactor.xxxy;

		// Assign tessellation levels
		output.Edges[0] = vEdgeTessellationFactors.x;
		output.Edges[1] = vEdgeTessellationFactors.y;
		output.Edges[2] = vEdgeTessellationFactors.z;
		output.Inside   = vEdgeTessellationFactors.w;
#if 0
		// Get the screen space position of each control point, so we can compute the 
		// desired tess factor based upon an ideal primitive size
		float2 f2EdgeScreenPosition0 = GetScreenSpacePosition( i_V0, g_vp,  g_targetRes.xy );
		float2 f2EdgeScreenPosition1 = GetScreenSpacePosition( i_V1, g_vp,  g_targetRes.xy );
		float2 f2EdgeScreenPosition2 = GetScreenSpacePosition( i_V2, g_vp,  g_targetRes.xy );

		output.Edges[0] = GetScreenSpaceAdaptiveTessFactor( f2EdgeScreenPosition2, f2EdgeScreenPosition0, 
			vEdgeTessellationFactors.x, g_vTessellationFactor.z );
		output.Edges[1] = GetScreenSpaceAdaptiveTessFactor( f2EdgeScreenPosition0, f2EdgeScreenPosition1, 
			vEdgeTessellationFactors.y, g_vTessellationFactor.z );
		output.Edges[2] = GetScreenSpaceAdaptiveTessFactor( f2EdgeScreenPosition1, f2EdgeScreenPosition2, 
			vEdgeTessellationFactors.z, g_vTessellationFactor.z );

		output.Inside = max(output.Edges[0], max(output.Edges[1], output.Edges[2]));
#endif
	}
	return output;
}

HS_CONSTANT_DATA_OUTPUT Constants_HS( InputPatch<TANGENT_TESS_OUTPUT, 3> inputPatch )
{
	return AdaptiveTessellate(inputPatch[0].V.WorldPos,
							  inputPatch[1].V.WorldPos,
							  inputPatch[2].V.WorldPos);
}

[domain("tri")]
[partitioning(SGPU_PARTITIONING)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("Constants_HS")]
TANGENT_TESS_OUTPUT Tangent_HS( InputPatch<TANGENT_TESS_OUTPUT, 3> inputPatch,uint uCPID : SV_OutputControlPointID )
{
	return inputPatch[uCPID];
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("tri")]
TANGENT_VERTEX_OUTPUT Tangent_DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
						 const OutputPatch<TANGENT_TESS_OUTPUT, 3> TrianglePatch, out float ClipDist : SV_ClipDistance0 )
{
	TANGENT_VERTEX_OUTPUT output = (TANGENT_VERTEX_OUTPUT)0;

	// Interpolate world space position with barycentric coordinates
	float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].V.WorldPos + 
		BarycentricCoordinates.y * TrianglePatch[1].V.WorldPos + 
		BarycentricCoordinates.z * TrianglePatch[2].V.WorldPos;

	float3 vWorldTanMatrixX = BarycentricCoordinates.x * TrianglePatch[0].V.WorldTan.X + 
		BarycentricCoordinates.y * TrianglePatch[1].V.WorldTan.X + 
		BarycentricCoordinates.z * TrianglePatch[2].V.WorldTan.X;
	float3 vWorldTanMatrixY = BarycentricCoordinates.x * TrianglePatch[0].V.WorldTan.Y + 
		BarycentricCoordinates.y * TrianglePatch[1].V.WorldTan.Y + 
		BarycentricCoordinates.z * TrianglePatch[2].V.WorldTan.Y;
	float3 vWorldTanMatrixZ = BarycentricCoordinates.x * TrianglePatch[0].V.WorldTan.Z + 
		BarycentricCoordinates.y * TrianglePatch[1].V.WorldTan.Z + 
		BarycentricCoordinates.z * TrianglePatch[2].V.WorldTan.Z;

	float2 UV = BarycentricCoordinates.x * TrianglePatch[0].V.UV + 
		BarycentricCoordinates.y * TrianglePatch[1].V.UV + 
		BarycentricCoordinates.z * TrianglePatch[2].V.UV;
	float2 TexCoord0 = BarycentricCoordinates.x * TrianglePatch[0].V.TexCoord0 + 
		BarycentricCoordinates.y * TrianglePatch[1].V.TexCoord0 + 
		BarycentricCoordinates.z * TrianglePatch[2].V.TexCoord0;


	float3 TanNormal = float3(0,0,1);

//	if( g_hasDisplacementMap )
//	{
//		TanNormal = ComputeNormalFromHeightMap(TexCoord0, g_DisplacementMap, g_DisplacementBlur*10.0f,
//								  4);
//		float3 TanNorm;
//		float height = SampleDisplacementTriCubic( TexCoord0, g_DisplacementBlur*10.0f, TanNorm );
//		vWorldPos = vWorldPos + (normalize(vWorldTanMatrixZ) * height);
//	}

#ifdef APPLY_DISPLACEMENT
	vWorldPos = DisplaceVertexNormal( vWorldPos, vWorldTanMatrixZ, TexCoord0, TanNormal );
#endif

	output.V.UV = UV;
	output.V.TexCoord0 = TexCoord0;

	float3x3 TanMat = {vWorldTanMatrixX, vWorldTanMatrixY, vWorldTanMatrixZ};

	// Rotate tangent frame to line up to the bumped normal (for subsequent displacements or normal-mapping)
	float3x3 mBumpRotation = MakeFromToRotationMatrixFast( normalize( vWorldTanMatrixZ ), TanNormal );

	// Convert normal to world space
	output.V.WorldTan.Z = mul( TanNormal, TanMat );
	//transform tangent frame
	output.V.WorldTan.X = mul( mBumpRotation, vWorldTanMatrixX );
	output.V.WorldTan.Y = mul( mBumpRotation, vWorldTanMatrixY );

	output.V.WorldPos = vWorldPos;

	// Transform world position with viewprojection matrix
	//output.HPosition = mul( g_vp, float4( vWorldPos, 1.0 ) );
	output.HPosition = TransformVertex(float4( vWorldPos, 1.0 ), output.V.UV, g_vp );

	float4 Ph = mul( g_vp, float4(vWorldPos,1.0f) );
	output.ScreenPos.x = 0.5 * (Ph.w + Ph.x);
	output.ScreenPos.y = 0.5 * (Ph.w + Ph.y);
	output.ScreenPos.z = Ph.w;

	ClipDist = ClipWorldPos( vWorldPos );

	return output;
}

#define TANGENT_HULL_AND_DOMAIN_Default

#define TANGENT_HULL_AND_DOMAIN_Tess\
	SetHullShader(CompileShader(hs_5_0, Tangent_HS()));\
	SetDomainShader(CompileShader(ds_5_0, Tangent_DS()));


//--------------------------------------------------------------------------
//----------------DOF tessellation support----------------------------------
//--------------------------------------------------------------------------

struct DOFTessOutput 
{
	float3 WorldPos		: TEXCOORD0;
	float3 WorldNorm	: TEXCOORD1;
	float2 TexCoord		: TEXCOORD2;
};


DOFTessOutput DOFPrepVS_Tess( STANDARD_VERTEX IN )
{
	DOFTessOutput OUT;
	// output position in proj space
	OUT.WorldPos = mul( g_world, float4( IN.Position, 1.0f) ).xyz;
	OUT.WorldNorm = mul( g_world, float4( IN.Position, 0.0f ) ).xyz;
	OUT.TexCoord = mul(g_uvTransform, float4(IN.UV,0,1)).xy;
	return OUT;
}

//--------------------------------------------------------------------------------------
// Hull shader
//--------------------------------------------------------------------------------------

HS_CONSTANT_DATA_OUTPUT DOF_Constants_HS( InputPatch<DOFTessOutput, 3> inputPatch )
{
	return AdaptiveTessellate(inputPatch[0].WorldPos,
		inputPatch[1].WorldPos,
		inputPatch[2].WorldPos);
}

[domain("tri")]
[partitioning(SGPU_PARTITIONING)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("DOF_Constants_HS")]
DOFTessOutput DOF_HS( InputPatch<DOFTessOutput, 3> inputPatch,uint uCPID : SV_OutputControlPointID )
{
	return inputPatch[uCPID];
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("tri")]
DOFvertexOutput DOF_DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
								 const OutputPatch<DOFTessOutput, 3> TrianglePatch, out float ClipDist : SV_ClipDistance0 )
{
	DOFvertexOutput output = (DOFvertexOutput)0;

	// Interpolate world space position with barycentric coordinates
	float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].WorldPos + 
		BarycentricCoordinates.y * TrianglePatch[1].WorldPos + 
		BarycentricCoordinates.z * TrianglePatch[2].WorldPos;

	float3 vWorldNorm = BarycentricCoordinates.x * TrianglePatch[0].WorldNorm + 
		BarycentricCoordinates.y * TrianglePatch[1].WorldNorm + 
		BarycentricCoordinates.z * TrianglePatch[2].WorldNorm;

	float2 TexCoord = BarycentricCoordinates.x * TrianglePatch[0].TexCoord + 
		BarycentricCoordinates.y * TrianglePatch[1].TexCoord +
		BarycentricCoordinates.z * TrianglePatch[2].TexCoord;

	float3 TanNormal = float3(0,0,1);
#ifdef APPLY_DISPLACEMENT
	vWorldPos = DisplaceVertex( vWorldPos, vWorldNorm, TexCoord );
#endif

	// Transform world position with viewprojection matrix
	//output.HPosition = mul( g_vp, float4( vWorldPos, 1.0 ) );
	output.HPosition = TransformVertex(float4( vWorldPos, 1.0 ), TexCoord, g_vp );

	output.ViewSpacePos = mul( g_view, float4( vWorldPos, 1.0 ) );

	ClipDist = ClipWorldPos( vWorldPos );

	return output;
}

#define DOF_HULL_AND_DOMAIN_Default

#define DOF_HULL_AND_DOMAIN_Tess\
	SetHullShader(CompileShader(hs_5_0, DOF_HS()));\
	SetDomainShader(CompileShader(ds_5_0, DOF_DS()));

/*********** eof **************/
