//////////////////////////////////////////////////////////////////////////////
// Converted from Solid.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// Solid.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Solid.fx
**
**      Fixed single color flat rendering for shader array
**
**	Gigawatt Studios
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

// Register layout shared with the materials: see ../Materials/Globals.hlsli.
#include "../Materials/Support.hlsli"
#include "../Materials/Tessellate.hlsli"

// b4: this shader's own parameters (defaults are in Solid.effect.json)
cbuffer MaterialParams : register(b4)
{
	float4 solidColor;				// default (0.5,0.5,0.5,1)
	float g_Transparency;			// : Opacity, default 1
	bool hasTransparencyMap;		// default false
};

Texture2D TransparencyMap : register(t4);		// : OpacityTexture

/************* DATA STRUCTS **************/

/* data passed from vertex shader to pixel shader */
struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
	float2 TexCoord0	: TEXCOORD0;
	float3 WorldPos		: TEXCOORD1;
};

struct VertexOutputTess
{
	float2 Texcoord : TEXCOORD0;
    float3 Position	: TEXCOORD1;
    float3 Normal	: TEXCOORD2;
};


/*********** vertex shader ******/

VertexOutput SolidVS_Default( STANDARD_VERTEX In, out float ClipDist : SV_ClipDistance0 )
{
    VertexOutput OUT;

    // Output vertex position
	float4 Po = float4(In.Position, 1.0f);
    OUT.HPosition = mul( g_wvp, Po);

	OUT.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;

	float3 Pw = mul( g_world, Po ).xyz;
	OUT.WorldPos = Pw;
	ClipDist = ClipWorldPos( Pw );

	return OUT;
}

VertexOutputTess SolidVS_Tess( STANDARD_VERTEX In )
{
    VertexOutputTess OUT;

    // Output vertex position
	float4 Po = float4(In.Position, 1.0f);
    OUT.Position = mul( g_world, Po).xyz;
	OUT.Normal = mul( (float3x3)g_world, In.Normal );
    OUT.Texcoord = mul( g_uvTransform, float4(In.UV,0,1)).xy;
	return OUT;
}

//--------------------------------------------------------------------------------------
// Pixel shader
//--------------------------------------------------------------------------------------

pixelOutput labPS( VertexOutput IN, uniform bool bColor )
{
    pixelOutput OUT; 

	OUT.col = bColor ? solidColor : float4(0,0,0,1);

	//alpha test
	float Alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, IN.TexCoord0, g_Transparency).r * OUT.col.a;
	if( g_AlphaTestRef >= Alpha ) discard;

    return OUT;
}

pixelOutput SolidDOFPrepTrans_PS(VertexOutput IN, uniform bool bHasTMap, uniform Texture2D i_TMap, uniform float i_Trans ) 
{
    pixelOutput OUT;

	//early alpha test
//	float Alpha = Tex2DCombine(bHasTMap, i_TMap, IN.TexCoord0, i_Trans).r;
//	if( g_AlphaTestRef >= Alpha ) discard;

    float bl = ComputeDepthBlur(length(IN.WorldPos.xyz - g_eyePos.xyz));//IN.ViewSpacePos.z); 
    OUT.col = float4(bl,bl,bl,bl);
    return OUT;
}

//--------------------------------------------------------------------------------------
// Hull shader
//--------------------------------------------------------------------------------------

HS_CONSTANT_DATA_OUTPUT Solid_Constants_HS( InputPatch<VertexOutputTess, 3> inputPatch )
{
	return AdaptiveTessellate(inputPatch[0].Position,
		inputPatch[1].Position,
		inputPatch[2].Position);
}

[domain("tri")]
[partitioning(SGPU_PARTITIONING)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("Solid_Constants_HS")]
VertexOutputTess Solid_HS( InputPatch<VertexOutputTess, 3> inputPatch, uint uCPID : SV_OutputControlPointID )
{
	return inputPatch[uCPID];
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("tri")]
VertexOutput Solid_DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
						 const OutputPatch<VertexOutputTess, 3> TrianglePatch, out float ClipDist : SV_ClipDistance0 )
{
	VertexOutput output = (VertexOutput)0;

	// Interpolate world space position with barycentric coordinates

	float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].Position + 
		BarycentricCoordinates.y * TrianglePatch[1].Position + 
		BarycentricCoordinates.z * TrianglePatch[2].Position;

	float2 TexCoord = BarycentricCoordinates.x * TrianglePatch[0].Texcoord + 
		BarycentricCoordinates.y * TrianglePatch[1].Texcoord + 
		BarycentricCoordinates.z * TrianglePatch[2].Texcoord;

	float3 vNormal  = BarycentricCoordinates.x * TrianglePatch[0].Normal + 
		BarycentricCoordinates.y * TrianglePatch[1].Normal + 
		BarycentricCoordinates.z * TrianglePatch[2].Normal;

	//need normal to displace
#ifdef APPLY_DISPLACEMENT
	vWorldPos = DisplaceVertex( vWorldPos, vNormal, TexCoord );
#endif

	// Transform world position with viewprojection matrix
	output.HPosition = mul( g_vp, float4( vWorldPos, 1.0 ) );
	output.WorldPos = vWorldPos;
	output.TexCoord0 = TexCoord;

	ClipDist = ClipWorldPos( vWorldPos );

	return output;
}



//--------------------------------------------------------------------------------------
// TECHNIQUES
//--------------------------------------------------------------------------------------


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput Default_PDefault_PS(VertexOutput IN)
{
    return labPS(IN, true);
}

pixelOutput SingleLight_PDefault_PS(VertexOutput IN)
{
    return labPS(IN, false);
}

pixelOutput DOFPrep_PDefault_PS(VertexOutput IN)
{
    return SolidDOFPrepTrans_PS(IN, hasTransparencyMap, TransparencyMap, g_Transparency);
}
