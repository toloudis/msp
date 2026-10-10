//////////////////////////////////////////////////////////////////////////////
// Converted from DepthRender.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// DepthRender.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  DepthRender.fx
**
**  Projected:    Render depth in pixel shader. Skips pixels where the texture's alpha is below a threshold.
**  View:  Renders actual depths.  Supports depth peeled layers (near to far).
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

#include "../Materials/Support.hlsli"
#include "../Materials/Tessellate.hlsli"

// Register layout shared with the materials: see ../Materials/Globals.hlsli.
// b4: this shader's own parameters (defaults are in DepthRender.effect.json)
cbuffer MaterialParams : register(b4)
{
	bool hasTransparencyMap;		// default false
	float g_Transparency;			// default 1

	bool hasDepthMap;				// default false

	//float g_alphaThreshold = 254.0f/255.0f;
	float ZNear;					// default 1
	float ZFar;						// default 1000
	float2 g_InvScreenSize;

	float g_bias;					// default 0
};

Texture2D TransparencyMap : register(t4);
Texture2D DepthMap : register(t5);

/************* DATA STRUCTS **************/

struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
    float  Depth		: TEXCOORD0; //Linear Depth (View Space)
    float2 texCoord		: TEXCOORD1; //model UV's
};

struct VertexOutputTess
{
    float3 Position		: TEXCOORD0;
	float3 Normal		: TEXCOORD1; //world space normal
    float2 texCoord		: TEXCOORD2; //model UV's
};

//-----------------------------------------------------------------------------
// Vertex Shaders
//-----------------------------------------------------------------------------

VertexOutputTess DepthVS_Tess( STANDARD_VERTEX In )
{
    VertexOutputTess OUT;

	float4 Po = float4(In.Position, 1.0f);
    OUT.Position = mul( g_world, Po ).xyz;
	OUT.Normal = mul( (float3x3)g_world, In.Normal );
    OUT.texCoord = mul(g_uvTransform, float4(In.UV,0,1)).xy;
    
    return OUT;
}

VertexOutput DepthVS_Default( STANDARD_VERTEX In, out float ClipDist : SV_ClipDistance0 )
{
    VertexOutput OUT;

	float4 Po = float4(In.Position, 1.0f);
    OUT.HPosition = mul( g_wvp, Po );
    OUT.Depth = mul( g_wv, Po ).z;
    OUT.texCoord = mul(g_uvTransform, float4(In.UV,0,1)).xy;

	float3 Pw = mul( g_world, Po ).xyz;
	ClipDist = ClipWorldPos( Pw );

    return OUT;
}

//-----------------------------------------------------------------------------
// Pixel Shaders
//-----------------------------------------------------------------------------


//Renders screen space depth
//Note: Only renders fully opaque pixels
pixelOutput DepthPS( VertexOutput In )
{
    pixelOutput OUT;

	//early alpha test
	float Alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.texCoord, g_Transparency).r;
	if( g_AlphaTestRef >= Alpha ) discard;
	
    OUT.col = In.HPosition.z + g_bias;

    return OUT;
}

//screen space depth (znear -> zfar) with depth peeling
//Note: Rejects fully transparent pixels
pixelOutput ScreenSpaceDepthPeelGreater_PS( VertexOutput In )
{
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.texCoord, g_Transparency).r;

	//half texel offset and buffer scaling
	float2 UV = In.HPosition.xy * g_InvScreenSize;

	float depth = In.HPosition.z;
	float prevDepth = DepthMap.Sample( g_DefaultSampler, UV ).r;

	float2 test;
	test.x = alpha - g_AlphaTestRef;		//alpha clip
	test.y = (depth - prevDepth) - g_bias;	//depth clip
	clip( test );

	OUT.col = depth;
    return OUT;
}

//screen space depth (zfar -> znear) with depth peeling
//Note: Rejects fully transparent pixels
pixelOutput ScreenSpaceDepthPeelLess_PS( VertexOutput In )
{
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.texCoord, g_Transparency).r;

	//half texel offset and buffer scaling
	float2 UV = In.HPosition.xy * g_InvScreenSize;

	float depth = In.HPosition.z;
	float prevDepth = DepthMap.Sample( g_DefaultSampler, UV ).r;

	float2 test;
	test.x = alpha - g_AlphaTestRef;		//alpha clip
	test.y = (prevDepth - depth) - g_bias;	//depth clip
	clip( test );

	OUT.col = depth;
    return OUT;
}

//actual view space depth (znear -> zfar)
//Note: only renders fully opaque pixels
pixelOutput ViewSpaceDepthPS( VertexOutput In )
{   
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.texCoord, g_Transparency).r;
	clip(alpha - g_AlphaTestRef);

    OUT.col = In.Depth;
    return OUT;
}


//actual view space depth (znear -> zfar) with depth peeling
//Note: Rejects fully transparent pixels
pixelOutput ViewSpaceDepthPeelLess_PS( VertexOutput In )
{
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.texCoord, g_Transparency).r;

	//half texel offset and buffer scaling
	float2 UV = In.HPosition.xy * g_InvScreenSize;

	float depth = In.Depth;
	float prevDepth = DepthMap.Sample( g_DefaultSampler, UV ).r;

//	float2 test;
//	test.x = alpha - g_AlphaTestRef;		//alpha clip
//	test.y = (depth - prevDepth);	//depth clip
//	clip( test );

	if( alpha < g_AlphaTestRef || depth <= prevDepth ) clip(-1);

	OUT.col = depth;
    return OUT;
}

//actual view space depth (zfar -> znear) with depth peeling (reversed)
//Note: Rejects fully transparent pixels
pixelOutput ViewSpaceDepthPeelGreater_PS( VertexOutput In )
{
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.texCoord, g_Transparency).r;

	//half texel offset and buffer scaling
	float2 UV = In.HPosition.xy * g_InvScreenSize;

	float depth = In.Depth;
	float prevDepth = DepthMap.Sample( g_DefaultSampler, UV ).r;

//	float2 test;
//	test.x = alpha - g_AlphaTestRef;		//alpha clip
//	test.y = (prevDepth - depth) - g_bias;	//depth clip
//	clip( test );

	if( alpha < g_AlphaTestRef || depth >= prevDepth ) clip(-1);

	OUT.col = depth;
    return OUT;
}


//normalized view space depth (0 -> 1)
//Note: Rejects fully transparent pixels
pixelOutput NViewSpaceDepth_PS( VertexOutput In )
{   
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.texCoord, g_Transparency).r;
	clip(alpha - g_AlphaTestRef);

    OUT.col = (In.Depth - ZNear) / (ZFar - ZNear);
    return OUT;
}

//normalized view space depth (0 -> 1) with depth peeling
//Note: Rejects fully transparent pixels
pixelOutput NViewSpaceDepthPeelLess_PS( VertexOutput In )
{
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.texCoord, g_Transparency).r;

	//half texel offset and buffer scaling
	float2 UV = In.HPosition.xy * g_InvScreenSize;

	float depth = (In.Depth - ZNear) / (ZFar - ZNear);
	float prevDepth = DepthMap.Sample( g_DefaultSampler, UV ).r;

	float2 test;
	test.x = alpha - g_AlphaTestRef;		//alpha clip
	test.y = (depth - prevDepth) - g_bias;	//depth clip
	clip( test );

//	if( alpha < alphaPass || depth <= prevDepth ) clip(-1);

	OUT.col = depth;
    return OUT;
}

//normalize view space depth (1 -> 0) with depth peeling (reversed)
//Note: Rejects fully transparent pixels
pixelOutput NViewSpaceDepthPeelGreater_PS( VertexOutput In )
{
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = Tex2DCombine(hasTransparencyMap, TransparencyMap, In.texCoord, g_Transparency).r;

	//half texel offset and buffer scaling
	float2 UV = In.HPosition.xy * g_InvScreenSize;

	float depth = (In.Depth - ZNear) / (ZFar - ZNear);
	float prevDepth = DepthMap.Sample( g_DefaultSampler, UV ).r;

	float2 test;
	test.x = alpha - g_AlphaTestRef;		//alpha clip
	test.y = (prevDepth - depth) - g_bias;	//depth clip
	clip( test );

//	if( alpha < alphaPass || depth >= prevDepth ) clip(-1);

	OUT.col = depth;
    return OUT;
}

pixelOutput DepthDOFPrepTrans_PS(VertexOutput IN, uniform bool bHasTMap, uniform Texture2D i_TMap, uniform float i_Trans ) 
{
    pixelOutput OUT;

	//early alpha test
//	float Alpha = Tex2DCombine(bHasTMap, i_TMap, IN.TexCoord0, i_Trans).r;
//	if( g_AlphaTestRef >= Alpha ) discard;

    float bl = ComputeDepthBlur(IN.Depth);//IN.ViewSpacePos.z); 
    OUT.col = float4(bl,bl,bl,bl);
    return OUT;
}

//--------------------------------------------------------------------------------------
// Hull shader
//--------------------------------------------------------------------------------------

HS_CONSTANT_DATA_OUTPUT Depth_Constants_HS( InputPatch<VertexOutputTess, 3> inputPatch )
{
	return AdaptiveTessellate(inputPatch[0].Position,
		inputPatch[1].Position,
		inputPatch[2].Position);
}

[domain("tri")]
[partitioning(SGPU_PARTITIONING)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("Depth_Constants_HS")]
VertexOutputTess Depth_HS( InputPatch<VertexOutputTess, 3> inputPatch, uint uCPID : SV_OutputControlPointID )
{
	return inputPatch[uCPID];
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("tri")]
VertexOutput Depth_DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
						 const OutputPatch<VertexOutputTess, 3> TrianglePatch, out float ClipDist : SV_ClipDistance0 )
{
	VertexOutput output = (VertexOutput)0;

	// Interpolate world space position with barycentric coordinates
	float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].Position + 
		BarycentricCoordinates.y * TrianglePatch[1].Position + 
		BarycentricCoordinates.z * TrianglePatch[2].Position;

	float2 TexCoord0 = BarycentricCoordinates.x * TrianglePatch[0].texCoord + 
		BarycentricCoordinates.y * TrianglePatch[1].texCoord + 
		BarycentricCoordinates.z * TrianglePatch[2].texCoord;

	float3 vNormal  = BarycentricCoordinates.x * TrianglePatch[0].Normal + 
		BarycentricCoordinates.y * TrianglePatch[1].Normal + 
		BarycentricCoordinates.z * TrianglePatch[2].Normal;

	//need normal to displace
#ifdef APPLY_DISPLACEMENT
	vWorldPos = DisplaceVertex( vWorldPos, vNormal, TexCoord0 );
#endif

    output.texCoord = TexCoord0;

	// Transform world position with viewprojection matrix
	output.HPosition = mul( g_vp, float4( vWorldPos, 1.0f ) );
	output.Depth = mul( g_view, float4( vWorldPos, 1.0f)).z;

	ClipDist = ClipWorldPos( vWorldPos );

	return output;
}


//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------


//non-linear screen space depths z = 0 -> 1 with depth peeling


//non-linear screen space depths z = 1 -> 0 with depth peeling


//actual view space depths z = near -> far


//actual view space depths z = near -> far with depth peeling


//actual view space depths z = far -> near with depth peeling (reversed)


//normalize view space depths z = 0 -> 1


//normalize view space depths z = 0 -> 1 with depth peeling


//normalize view space depths z = 1 -> 0 with depth peeling (reversed)


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput DOFPrep_PDefault_PS(VertexOutput IN)
{
    return DepthDOFPrepTrans_PS(IN, hasTransparencyMap, TransparencyMap, g_Transparency);
}
