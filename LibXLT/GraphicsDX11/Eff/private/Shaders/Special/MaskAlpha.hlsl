//////////////////////////////////////////////////////////////////////////////
// Converted from MaskAlpha.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// MaskAlpha.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  MaskAlpha.fx
**
**      Fixed single color flat rendering for shader array,
**	skips pixels where the texture's alpha is below a threshold.
**
**	Gigawatt Studios
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

// Register layout shared with the materials: see ../Materials/Globals.hlsli.
#include "../Materials/Support.hlsli"
#include "../Materials/Tessellate.hlsli"

// b4: this shader's own parameters (defaults are in MaskAlpha.effect.json)
cbuffer MaterialParams : register(b4)
{
	float4 solidColor;				// default (0.5,0.5,0.5,1)
	bool hasDiffuseMap;				// default false
};

Texture2D diffuseMap : register(t4);
//float alphaThreshold = 0.1;

/************* DATA STRUCTS **************/

struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
    float2 TexCoord0	: TEXCOORD0;
    float  Depth		: TEXCOORD1;		//linear view space depth
    float4 diffCol		: COLOR0;
};

struct VertexOutputTess
{
    float2 TexCoord0	: TEXCOORD0;
    float4 diffCol		: COLOR0;
    float3 Position		: TEXCOORD2;
	float3 Normal		: TEXCOORD3;
};

/*********** vertex shader ******/

VertexOutput MaskVS_Default( STANDARD_VERTEX In, uniform bool bUseColor, uniform bool bTransformUVs, out float ClipDist : SV_ClipDistance0 )
{
    VertexOutput OUT;
    
    // Always black in single light, all color comes from ambient pass
	OUT.diffCol = bUseColor ? solidColor : float4(0,0,0,1);
    
    // Single texture layer
	OUT.TexCoord0 = bTransformUVs ? mul(g_uvTransform, float4(In.UV,0,1)).xy : In.UV;
    
    // Output vertex position
	float4 Po = float4(In.Position, 1.0f);
    OUT.HPosition = mul( g_wvp, Po );
    OUT.Depth = mul( g_wv, Po).z;

	float3 Pw = mul( g_world, Po ).xyz;
	ClipDist = ClipWorldPos( Pw );
    
    return OUT;
}

VertexOutputTess MaskVS_Tess( STANDARD_VERTEX In, uniform bool bUseColor, uniform bool bTransformUVs )
{
    VertexOutputTess OUT;
    
    // Always black in single light, all color comes from ambient pass
	OUT.diffCol = bUseColor ? solidColor : float4(0,0,0,1);
    
    // Single texture layer
	OUT.TexCoord0 = bTransformUVs ? mul(g_uvTransform, float4(In.UV,0,1)).xy : In.UV;
    
    // Output vertex position
	float4 Po = float4(In.Position, 1.0f);
    OUT.Position = mul( g_world, Po ).xyz;
    OUT.Normal = mul( (float3x3)g_world, In.Normal );
    
    return OUT;
}

/********* pixel shader ********/

pixelOutput labPS( VertexOutput IN,
				  uniform Texture2D DiffuseMap) 
{
    pixelOutput OUT; 
    
    if (hasDiffuseMap)
    {
		// Clip pixel if the alpha value is below a threshold
		float alpha = DiffuseMap.Sample(g_DefaultSampler, IN.TexCoord0.xy).a;
		clip(alpha - g_AlphaTestRef); // clip skips pixel if value is negative
	}
	
    OUT.col = IN.diffCol;
    return OUT;
}

pixelOutput pickPS( VertexOutput IN,
				  uniform Texture2D DiffuseMap) 
{
    pixelOutput OUT; 
    
    if (hasDiffuseMap)
    {
		// Clip pixel if the alpha value is below a threshold
		float alpha = DiffuseMap.Sample(g_DefaultSampler, IN.TexCoord0.xy).a;
		clip(alpha - g_AlphaTestRef); // clip skips pixel if value is negative
	}
	
    OUT.col = float4(IN.diffCol.x, IN.TexCoord0.x, IN.TexCoord0.y, IN.Depth);
    return OUT;
}

//--------------------------------------------------------------------------------------
// Hull shader
//--------------------------------------------------------------------------------------

HS_CONSTANT_DATA_OUTPUT Mask_Constants_HS( InputPatch<VertexOutputTess, 3> inputPatch )
{
	return AdaptiveTessellate(inputPatch[0].Position,
		inputPatch[1].Position,
		inputPatch[2].Position);
}

[domain("tri")]
[partitioning(SGPU_PARTITIONING)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("Mask_Constants_HS")]
VertexOutputTess Mask_HS( InputPatch<VertexOutputTess, 3> inputPatch, uint uCPID : SV_OutputControlPointID )
{
	return inputPatch[uCPID];
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("tri")]
VertexOutput Mask_DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
						 const OutputPatch<VertexOutputTess, 3> TrianglePatch, out float ClipDist : SV_ClipDistance0 )
{
	VertexOutput output = (VertexOutput)0;

	// Interpolate world space position with barycentric coordinates

	float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].Position + 
		BarycentricCoordinates.y * TrianglePatch[1].Position + 
		BarycentricCoordinates.z * TrianglePatch[2].Position;

	float2 TexCoord0 = BarycentricCoordinates.x * TrianglePatch[0].TexCoord0 + 
		BarycentricCoordinates.y * TrianglePatch[1].TexCoord0 + 
		BarycentricCoordinates.z * TrianglePatch[2].TexCoord0;

	float4 vDiffCol  = BarycentricCoordinates.x * TrianglePatch[0].diffCol + 
		BarycentricCoordinates.y * TrianglePatch[1].diffCol + 
		BarycentricCoordinates.z * TrianglePatch[2].diffCol;

	float3 vNormal  = BarycentricCoordinates.x * TrianglePatch[0].Normal + 
		BarycentricCoordinates.y * TrianglePatch[1].Normal + 
		BarycentricCoordinates.z * TrianglePatch[2].Normal;

	//need normal to displace
#ifdef APPLY_DISPLACEMENT
	vWorldPos = DisplaceVertex( vWorldPos, vNormal, TexCoord0 );
#endif

    output.diffCol = vDiffCol;
	output.TexCoord0 = TexCoord0;
	output.Depth = mul( g_view, float4( vWorldPos, 1.0f)).z;

	// Transform world position with viewprojection matrix
	output.HPosition = mul( g_vp, float4( vWorldPos, 1.0 ) );

	ClipDist = ClipWorldPos( vWorldPos );

	return output;
}


//-----------------------------------------------------------------------------
// TECHNIQUES
//-----------------------------------------------------------------------------


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

VertexOutput Default_PDefault_VS(STANDARD_VERTEX In, out float ClipDist : SV_ClipDistance0)
{
    return MaskVS_Default(In, true, false, ClipDist);
}

pixelOutput Default_PDefault_PS(VertexOutput IN)
{
    return labPS(IN, diffuseMap);
}

VertexOutputTess Default_PTess_VS(STANDARD_VERTEX In)
{
    return MaskVS_Tess(In, true, false);
}

VertexOutput SingleLight_PDefault_VS(STANDARD_VERTEX In, out float ClipDist : SV_ClipDistance0)
{
    return MaskVS_Default(In, false, false, ClipDist);
}

VertexOutputTess SingleLight_PTess_VS(STANDARD_VERTEX In)
{
    return MaskVS_Tess(In, false, false);
}

VertexOutput Environment_PDefault_VS(STANDARD_VERTEX In, out float ClipDist : SV_ClipDistance0)
{
    return MaskVS_Default(In, true, true, ClipDist);
}

pixelOutput Environment_PDefault_PS(VertexOutput IN)
{
    return pickPS(IN, diffuseMap);
}

VertexOutputTess Environment_PTess_VS(STANDARD_VERTEX In)
{
    return MaskVS_Tess(In, true, true);
}
