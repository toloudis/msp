//////////////////////////////////////////////////////////////////////////////
// Converted from HairDefault.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// HairDefault.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  HairDefault.fx
**
**      Internal techniques for supporting various hair rendering
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
**
**  Note: Geometry is assumed to be line segments where the normal is
**   tangent to the line direction and w component is a radius expansion amount.
\****************************************************************************/

// The SasGlobal effect description (gp : SasGlobal) is in
// HairDefault.effect.json. This effect is driven by the same C++ shader
// classes as the materials, so it uses the material binding model (see
// Materials/Globals.hlsli) and the hair resources of
// Materials/SupportHair.hlsli (b5 HairParams, t26-t38, u1, s10).

/*********** support data and functions ******/

#include "../Materials/Support.hlsli"
#include "../Materials/SupportHair.hlsli"

// b4: this effect's own parameters (defaults are in HairDefault.effect.json)
cbuffer MaterialParams : register(b4)
{
	float4 solidColor;				// default (1,1,1,1): for solid shader
	bool g_UseCosine;				// default true
	//float g_alphaThreshold;		// = 254.0f/255.0f
	float g_Transparency;			// default 1
	float g_bias;					// default 0
};

static const float alphaPass = 1.0f/255.0f;

/************* DATA STRUCTS **************/
struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
    float4 VPos			: TEXCOORD0; //View position
	float4 Color		: COLOR0;
	nointerpolation uint Segment	: TEXCOORD2;	//use to cull out degenerate strands
};

struct GeometryOutput
{
    float4 HPosition	: SV_POSITION;
    float4 VPos			: TEXCOORD0; //View position
	float4 Color		: COLOR0;
	float  ClipDist     : SV_ClipDistance0;
};

struct PixelInput
{
    float4 HPosition	: SV_POSITION;
    float4 VPos			: TEXCOORD0; //View position
	float4 Color		: COLOR0;
};

struct hairGeometryLit
{
    float4 HPosition	: SV_POSITION;
	float3 WPosition	: TEXCOORD0;
	float3 WTangent     : TEXCOORD1;
	float  ClipDist     : SV_ClipDistance0;
};

struct hairLitData
{
    float4 HPosition	: SV_POSITION;
	float3 WPosition	: TEXCOORD0;
	float3 WTangent     : TEXCOORD1;
};

//------simple vertex shader
VertexOutput SimpleVS_NoTess( uint VertID : SV_VertexID )
{
    VertexOutput OUT;

	uint nStrands = g_nStrandCPs-1;
	uint curSegment = VertID % g_nStrandCPs;
	uint curStrand = VertID / g_nStrandCPs;

	hairMaterial Mtl = g_HairMaterials[ curStrand ];

	//calculate hair vertex indices
	uint strandBaseID = curStrand * g_nStrandCPs;
	uint CPBase = strandBaseID + curSegment;

	float3 Pos = g_HairGeometry[ CPBase ];
	float3 Tan;

	//use next vertex if not at the end otherwise use previous
	if( curSegment >= nStrands )
	{
		Tan = Pos - g_HairGeometry[ CPBase-1 ];
	}
	else
	{
		Tan = g_HairGeometry[ CPBase+1 ] - Pos;
	}

	float3 vWorldPos = mul( g_world, float4( Pos, 1)).xyz;
	float3 vWorldTan = mul( (float3x3)g_world, Tan );

	float t = curSegment / (float)nStrands;

	OUT.Color.rgb = lerp( Mtl.m_RootColor, Mtl.m_TipColor, t);
	OUT.Color.a = 1.0f;
	float radius = lerp( Mtl.m_RootRadius, Mtl.m_TipRadius, t );

	OUT.HPosition = float4( vWorldPos, abs(radius) );
	OUT.VPos = float4( vWorldTan, t );
	OUT.Segment = curSegment;

    return OUT;
}

//NULL vertex shader
void SimpleVS_Tess(){}

//------------------Simple Pixel Shader
pixelOutput SimplePS( PixelInput IN )
{
    pixelOutput OUT; 
    
    OUT.col = IN.Color;

    return OUT;
}

//------------------Depth Pixel Shaders------------------------------------------
//Renders screen space depth
//Note: Only renders fully opaque pixels
pixelOutput DepthPS( PixelInput In )
{
    pixelOutput OUT; 
    
	// Clip pixel if the alpha value is below a threshold
	float alpha = In.Color.a * g_Transparency;
	clip(alpha - g_AlphaTestRef);
	
    OUT.col = In.HPosition.z + g_bias;

    return OUT;
}

//non-linear screen space depth (0 -> 1) with depth peeling
//Note: Rejects fully transparent pixels
pixelOutput ScreenSpaceDepthPeelGreater_PS( PixelInput In )
{
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = In.Color.a * g_Transparency;

	//half texel offset and buffer scaling
	float2 UV = In.HPosition.xy * g_InvScreenSize;

	float depth = In.HPosition.z;
	float prevDepth = depthMap.Sample( HairSampler, UV ).r;

	float2 test;
	test.x = alpha - g_AlphaTestRef;		//alpha clip
	test.y = (depth - prevDepth) - g_bias;	//depth clip
	clip( test );

	OUT.col = depth;
    return OUT;
}

//non-linear screen space depth (1 -> 0) with depth peeling (reversed)
//Note: Rejects fully transparent pixels
pixelOutput ScreenSpaceDepthPeelLess_PS( PixelInput In )
{
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = In.Color.a * g_Transparency;

	//half texel offset and buffer scaling
	float2 UV = In.HPosition.xy * g_InvScreenSize;

	float depth = In.HPosition.z;
	float prevDepth = depthMap.Sample( HairSampler, UV ).r;

	float2 test;
	test.x = alpha - g_AlphaTestRef;		//alpha clip
	test.y = (prevDepth - depth) - g_bias;	//depth clip
	clip( test );

	OUT.col = depth;
    return OUT;
}

//actual view space depth (znear -> zfar)
//Note: Only renders fully opaque pixels
pixelOutput ViewSpaceDepthPS( PixelInput In )
{   
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = In.Color.a * g_Transparency;
	clip(alpha - g_AlphaTestRef);

    OUT.col = In.VPos.z;
    return OUT;
}

//normalized view space depth (0 -> 1)
//Note: Only renders fully opaque pixels
pixelOutput ViewSpaceDepthNPS( PixelInput In )
{   
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = In.Color.a * g_Transparency;
	clip(alpha - g_AlphaTestRef);

    OUT.col = (In.VPos.z - g_ZNear) / (g_ZFar - g_ZNear);
    return OUT;
}

//actual view space depth (znear -> zfar) with depth peeling
//Note: Rejects fully transparent pixels
pixelOutput ViewSpaceDepthPeelPS( PixelInput In )
{
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = In.Color.a * g_Transparency;

	//half texel offset and buffer scaling
	float2 UV = In.HPosition.xy * g_InvScreenSize;

	float depth = In.VPos.z;
	float prevDepth = depthMap.Sample( HairSampler, UV ).r;

	if( alpha < alphaPass || depth <= prevDepth ) clip(-1);

	OUT.col = depth;
    return OUT;
}

//actual view space depth (zfar -> znear) with depth peeling (reversed)
//Note: Rejects fully transparent pixels
pixelOutput ViewSpaceDepthPeelRPS( PixelInput In )
{
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = In.Color.a * g_Transparency;

	//half texel offset and buffer scaling
	float2 UV = In.HPosition.xy * g_InvScreenSize;

	float depth = In.VPos.z;
	float prevDepth = depthMap.Sample( HairSampler, UV ).r;

	if( alpha < alphaPass || depth >= prevDepth ) clip(-1);

	OUT.col = depth;
    return OUT;
}

//normalized view space depth (0 -> 1) with depth peeling
//Note: Rejects fully transparent pixels
pixelOutput ViewSpaceDepthPeelNPS( PixelInput In )
{
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = In.Color.a * g_Transparency;

	//half texel offset and buffer scaling
	float2 UV = In.HPosition.xy * g_InvScreenSize;

	float depth = (In.VPos.z - g_ZNear) / (g_ZFar - g_ZNear);
	float prevDepth = depthMap.Sample( HairSampler, UV ).r;

	if( alpha < alphaPass || depth <= prevDepth ) clip(-1);

	OUT.col = depth;
    return OUT;
}

//normalize view space depth (1 -> 0) with depth peeling (reversed)
//Note: Rejects fully transparent pixels
pixelOutput ViewSpaceDepthPeelNRPS( PixelInput In )
{
    pixelOutput OUT = (pixelOutput)0;

	// Clip pixel if the alpha value is below a threshold
	float alpha = In.Color.a * g_Transparency;

	//half texel offset and buffer scaling
	float2 UV = In.HPosition.xy * g_InvScreenSize;

	float depth = (In.VPos.z - g_ZNear) / (g_ZFar - g_ZNear);
	float prevDepth = depthMap.Sample( HairSampler, UV ).r;

	if( alpha < alphaPass || depth >= prevDepth + g_bias ) clip(-1);

	OUT.col = depth;
    return OUT;
}

//------------------Tangent Pixel Shader------------------------------------------

pixelOutput NormalPS( PixelInput In )
{   
    pixelOutput OUT = (pixelOutput)0;

	OUT.col.xyz = In.VPos.xyz;
    
    // this ensures that the normals are orthogonal to the scene geometry, using eye space position.
//    float3 Ng = normalize(cross(ddx(In.EyePos.xyz), ddy(In.EyePos.xyz)));
//    Out.Color.xyz = Ng * 0.5 + 0.5;

    OUT.col.w = 1;//In.EyePos.z;
    
    return OUT;
}

//------------------Solid Pixel Shader------------------------------------------
pixelOutput SolidPS( PixelInput In )
{   
    pixelOutput OUT = (pixelOutput)0;
	OUT.col = solidColor;
    return OUT;
}


//-----------------Illumination and Shadow Pixel Shaders-----------------------------
pixelOutput singleLightPS( hairLitData IN, uniform LightInfo i_Light )
{
    pixelOutput OUT; 
    
	IncidentLight light;
	IlluminatePointLight( IN.WPosition, i_Light, light);
	
	float cosine = 1;
	if (g_UseCosine)
	{
		float3 lightDir = normalize(light.L);
		float3 worldEyeDir = normalize(IN.WPosition - g_eyePos.xyz);
		float3 WorldTan = normalize(IN.WTangent);

		cosine = cos( abs( acos(dot(WorldTan, lightDir)) - acos(-dot(WorldTan,worldEyeDir)) ) );
	}

	OUT.col.rgb = light.Cld * cosine;
	OUT.col.a = 1;
    return OUT;
}

pixelOutput projLightPS( hairLitData IN,		
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap )
{
    pixelOutput OUT = (pixelOutput)0;
	IncidentLight light;
	IlluminateProjLightHair( IN.WPosition, i_Light, i_ProjLight, g_bHasProjMap, ProjTextureMap, 1.0f, 0.0f, light);

	float cosine = 1;
	if (g_UseCosine)
	{
		float3 lightDir = normalize(light.L);
		float3 worldEyeDir = normalize(IN.WPosition - g_eyePos.xyz);
		float3 WorldTan = normalize(IN.WTangent);

		cosine = cos( abs( acos(dot(WorldTan, lightDir)) - acos(-dot(WorldTan,worldEyeDir)) ) );
	}

	OUT.col.rgb = light.Cld * cosine;
	OUT.col.a = 1;
    return OUT;
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("isoline")]
VertexOutput Hair_DS( HS_HAIR_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, 
					 const OutputPatch<hairOutput, 4> CP, uint PatchID : SV_PrimitiveID )
{
	VertexOutput output = (VertexOutput)0;
	float3 vWorldPos;
	float3 vWorldTan;

	float t = InterpolateHairStrand( CP, PatchID, UV, vWorldPos, vWorldTan );
	hairMaterial Mtl = GetMaterial( PatchID );

	float radius = lerp( Mtl.m_RootRadius, Mtl.m_TipRadius, t );
	output.Color.rgb = lerp( Mtl.m_RootColor, Mtl.m_TipColor, t);
	output.Color.a = Mtl.m_Opacity * g_Transparency;
	output.HPosition = float4( vWorldPos, abs(radius) );
	output.VPos = float4( vWorldTan, t );
	output.Segment = PatchID % g_nStrandCPs;

	return output;
}

[domain("isoline")]
PixelInput HairLines_DS( HS_HAIR_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, 
					 const OutputPatch<hairOutput, 4> CP, uint PatchID : SV_PrimitiveID )
{
	PixelInput output = (PixelInput)0;
	float3 vWorldPos;
	float3 vWorldTan;

	float t = InterpolateHairStrand( CP, PatchID, UV, vWorldPos, vWorldTan );
	hairMaterial Mtl = GetMaterial( PatchID );

	output.Color.rgb = lerp( Mtl.m_RootColor, Mtl.m_TipColor, t);
	output.Color.a = Mtl.m_Opacity * g_Transparency;
	output.HPosition = mul( g_vp, float4( vWorldPos, 1) );
    output.VPos = mul( g_view, float4( vWorldPos, 1) );

	return output;
}


[domain("isoline")]
hairGeometryLit HairLinesLit_DS( HS_HAIR_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, 
					 const OutputPatch<hairOutput, 4> CP, uint PatchID : SV_PrimitiveID )
{
	hairGeometryLit output = (hairGeometryLit)0;
	float3 vWorldPos;
	float3 vWorldTan;

	InterpolateHairStrand( CP, PatchID, UV, vWorldPos, vWorldTan );

	output.HPosition = mul( g_vp, float4( vWorldPos, 1) );
	output.WPosition = vWorldPos;
	output.WTangent = vWorldTan;
	output.ClipDist = ClipWorldPos( vWorldPos );

	return output;
}

//--------------------------------------------------------------------------------------
// Geometry Shader
//--------------------------------------------------------------------------------------
[maxvertexcount(4)]
void Hair_GS( line VertexOutput input[2], inout TriangleStream< GeometryOutput > TriStream )
{
	GeometryOutput output = (GeometryOutput)0;

	if( input[0].Segment >= (uint)(g_nStrandCPs-1) ) return;	//cull out degenerate segments

	for( uint i = 0; i < 2; i++ )
	{
		float radius = input[i].HPosition.w;
		float3 newPos = input[i].HPosition.xyz;
		float3 WTan = input[i].VPos.xyz;

		float3 Offset;
		float SubPixel = HairLimitExpand( newPos, WTan, radius, Offset );

		float3 WPos = newPos + (Offset * radius);	//position.w is signed radius
		output.HPosition = mul( g_vp, float4( WPos, 1) );
	    output.VPos = mul( g_view, float4(WPos,1) );
		output.Color = input[i].Color * SubPixel;
		output.ClipDist = ClipWorldPos( WPos );
		TriStream.Append( output );

		WPos = newPos + (Offset * -radius);	//position.w is signed radius
		output.HPosition = mul( g_vp, float4( WPos, 1) );
	    output.VPos = mul( g_view, float4(WPos,1) );
		output.ClipDist = ClipWorldPos( WPos );
		TriStream.Append( output );
	}

    TriStream.RestartStrip();
}

[maxvertexcount(4)]
void HairLit_GS( line VertexOutput input[2], inout TriangleStream<hairGeometryLit> TriStream )
{
	hairGeometryLit output;

	if( input[0].Segment >= (uint)(g_nStrandCPs-1) ) return;	//cull out degenerate segments

	for( uint i = 0; i < 2; i++ )
	{
		float radius = input[i].HPosition.w;
		float3 newPos = input[i].HPosition.xyz;
		float3 WTan = input[i].VPos.xyz;
		float3 Offset;
		HairLimitExpand( newPos, WTan, radius, Offset );

		float3 WPos = newPos + (Offset * radius);	//position.w is signed radius
		output.HPosition = mul( g_vp, float4( WPos, 1) );
		output.WPosition = WPos;
		output.WTangent = WTan;
		output.ClipDist = ClipWorldPos( WPos );
		TriStream.Append( output );

		WPos = newPos + (Offset * -radius);	//position.w is signed radius
		output.HPosition = mul( g_vp, float4( WPos, 1) );
		output.WPosition = WPos;
		output.WTangent = WTan;
		output.ClipDist = ClipWorldPos( WPos );
		TriStream.Append( output );
	}

    TriStream.RestartStrip();
}

[maxvertexcount(2)]
void HairLines_GS( line VertexOutput input[2], inout LineStream< GeometryOutput > Lines )
{
	GeometryOutput output = (GeometryOutput)0;

	if( input[0].Segment >= (uint)(g_nStrandCPs-1) ) return;	//cull out degenerate segments

	for( uint i = 0; i < 2; i++ )
	{
		float3 WPos = input[i].HPosition.xyz;
		output.HPosition = mul( g_vp, float4( WPos, 1) );
		output.VPos = mul( g_view, float4(WPos,1) );
		output.Color = input[i].Color;
		output.ClipDist = ClipWorldPos( WPos );
		Lines.Append( output );
	}

    Lines.RestartStrip();
}

[maxvertexcount(2)]
void HairLinesLit_GS( line VertexOutput input[2], inout LineStream<hairGeometryLit> Lines )
{
	hairGeometryLit output;

	if( input[0].Segment >= (uint)(g_nStrandCPs-1) ) return;	//cull out degenerate segments

	for( uint i = 0; i < 2; i++ )
	{
		float3 WPos = input[i].HPosition.xyz;
		float3 WTan = input[i].VPos.xyz;

		output.HPosition = mul( g_vp, float4( WPos, 1) );
		output.WPosition = WPos;
		output.WTangent = WTan;
		output.ClipDist = ClipWorldPos( WPos );
		Lines.Append( output );
	}

    Lines.RestartStrip();
}

#define HAIR_HULL_AND_DOMAIN_Default\
	SetVertexShader(CompileShader( vs_5_0, SimpleVS_NoTess()));\
	SetGeometryShader(CompileShader(gs_5_0, Hair_GS()));

#define HAIR_HULL_AND_DOMAIN_Tess\
	SetVertexShader(CompileShader( vs_5_0, SimpleVS_Tess()));\
	SetHullShader(CompileShader(hs_5_0, Hair_HS()));\
	SetDomainShader(CompileShader(ds_5_0, Hair_DS()));\
	SetGeometryShader(CompileShader(gs_5_0, Hair_GS()));

#define HAIR_HULL_AND_DOMAIN_Lines\
	SetVertexShader(CompileShader( vs_5_0, SimpleVS_NoTess()));\
	SetGeometryShader(CompileShader(gs_5_0, HairLines_GS()));

#define HAIR_HULL_AND_DOMAIN_LinesTess\
	SetVertexShader(CompileShader( vs_5_0, SimpleVS_Tess()));\
	SetHullShader(CompileShader(hs_5_0, Hair_HS()));\
	SetDomainShader(CompileShader(ds_5_0, HairLines_DS()));\


#define HAIR_LIT_HULL_AND_DOMAIN_Default\
	SetVertexShader(CompileShader( vs_5_0, SimpleVS_NoTess()));\
	SetGeometryShader(CompileShader(gs_5_0, HairLit_GS()));

#define HAIR_LIT_HULL_AND_DOMAIN_Tess\
	SetVertexShader(CompileShader( vs_5_0, SimpleVS_Tess()));\
	SetHullShader(CompileShader(hs_5_0, Hair_HS()));\
	SetDomainShader(CompileShader(ds_5_0, Hair_DS()));\
	SetGeometryShader(CompileShader(gs_5_0, HairLit_GS()));

#define HAIR_LIT_HULL_AND_DOMAIN_Lines\
	SetVertexShader(CompileShader( vs_5_0, SimpleVS_NoTess()));\
	SetGeometryShader(CompileShader(gs_5_0, HairLinesLit_GS()));\

#define HAIR_LIT_HULL_AND_DOMAIN_LinesTess\
	SetVertexShader(CompileShader( vs_5_0, SimpleVS_Tess()));\
	SetHullShader(CompileShader(hs_5_0, Hair_HS()));\
	SetDomainShader(CompileShader(ds_5_0, HairLinesLit_DS()));


//--------techniques for depth rendering


//non-linear screen space depths z = 0 -> 1 with depth peeling


//non-linear screen space depths z = 1 -> 0 with depth peeling


//actual view space depths z = near -> far


//normalize view space depths z = 0 -> 1


//actual view space depths z = near -> far with depth peeling


//actual view space depths z = far -> near with depth peeling (reversed)


//normalize view space depths z = 0 -> 1 with depth peeling


//normalize view space depths z = 1 -> 0 with depth peeling (reversed)


//--------techniques for viewing tangents (use in normal renderer)


//--------techniques for Material renderer (use in normal renderer)


//------Techniques for illumination and shader renderers


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput SingleLight_PDefault_PS(hairLitData IN)
{
    return singleLightPS(IN, g_lightInfo);
}

pixelOutput ProjectedLight_PDefault_PS(hairLitData IN)
{
    return projLightPS(IN, g_lightInfo, g_projLight, projLightMap);
}
