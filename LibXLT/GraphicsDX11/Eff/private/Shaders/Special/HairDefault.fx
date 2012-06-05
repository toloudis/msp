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

int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "John Schwab";
  string SasEffectAuthoringSoftware = "MachStudio";
  string SasEffectCategory			= "/material";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "HairDefault";
  string SasEffectHelp				= "This is hair shader defaults.";
  string SasEffectRevision			= "1";  
>;

/*********** support data and functions ******/

#include "..\Support.h"
#include "..\SupportHair.h"

float4 solidColor = {1,1,1,1};	//for solid shader

bool g_UseCosine = true;

static const float alphaPass = 1.0f/255.0f;

//float g_alphaThreshold = 254.0f/255.0f;
float g_Transparency = 1.0f;
float g_bias = 0.0f;

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
technique11 Simple
{
#define PASS_SIMPLE(PassName)	\
	pass P##PassName			\
	{						\
		SetPixelShader(CompileShader( ps_5_0, SimplePS()));\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_SIMPLE(Default)
PASS_SIMPLE(Tess)
PASS_SIMPLE(Lines)
PASS_SIMPLE(LinesTess)
}

technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		PixelShader = compile ps_5_0 DepthPS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_DEFAULT(Default)
PASS_DEFAULT(Tess)
PASS_DEFAULT(Lines)
PASS_DEFAULT(LinesTess)
}

//non-linear screen space depths z = 0 -> 1 with depth peeling
technique11 ScreenSpaceDepthPeelGreater
{
#define PASS_SCREENSPACEDEPTHPEELGREATER(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 ScreenSpaceDepthPeelGreater_PS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_SCREENSPACEDEPTHPEELGREATER(Default)
PASS_SCREENSPACEDEPTHPEELGREATER(Tess)
PASS_SCREENSPACEDEPTHPEELGREATER(Lines)
PASS_SCREENSPACEDEPTHPEELGREATER(LinesTess)
}

//non-linear screen space depths z = 1 -> 0 with depth peeling
technique11 ScreenSpaceDepthPeelLess
{
#define PASS_SCREENSPACEDEPTHPEELLESS(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 ScreenSpaceDepthPeelLess_PS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_SCREENSPACEDEPTHPEELLESS(Default)
PASS_SCREENSPACEDEPTHPEELLESS(Tess)
PASS_SCREENSPACEDEPTHPEELLESS(Lines)
PASS_SCREENSPACEDEPTHPEELLESS(LinesTess)
}

//actual view space depths z = near -> far
technique11 ViewSpaceDepth
{
#define PASS_VIEWSPACEDEPTH(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 ViewSpaceDepthPS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_VIEWSPACEDEPTH(Default)
PASS_VIEWSPACEDEPTH(Tess)
PASS_VIEWSPACEDEPTH(Lines)
PASS_VIEWSPACEDEPTH(LinesTess)
}

//normalize view space depths z = 0 -> 1
technique11 ViewSpaceDepthN
{
#define PASS_VIEWSPACEDEPTHN(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 ViewSpaceDepthNPS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_VIEWSPACEDEPTHN(Default)
PASS_VIEWSPACEDEPTHN(Tess)
PASS_VIEWSPACEDEPTHN(Lines)
PASS_VIEWSPACEDEPTHN(LinesTess)
}

//actual view space depths z = near -> far with depth peeling
technique11 ViewSpaceDepthPeel
{
#define PASS_VIEWSPACEDEPTHPEEL(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 ViewSpaceDepthPeelPS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_VIEWSPACEDEPTHPEEL(Default)
PASS_VIEWSPACEDEPTHPEEL(Tess)
PASS_VIEWSPACEDEPTHPEEL(Lines)
PASS_VIEWSPACEDEPTHPEEL(LinesTess)
}

//actual view space depths z = far -> near with depth peeling (reversed)
technique11 ViewSpaceDepthPeelR
{
#define PASS_VIEWSPACEDEPTHPEELR(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 ViewSpaceDepthPeelRPS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_VIEWSPACEDEPTHPEELR(Default)
PASS_VIEWSPACEDEPTHPEELR(Tess)
PASS_VIEWSPACEDEPTHPEELR(Lines)
PASS_VIEWSPACEDEPTHPEELR(LinesTess)
}

//normalize view space depths z = 0 -> 1 with depth peeling
technique11 ViewSpaceDepthPeelN
{
#define PASS_VIEWSPACEDEPTHPEELN(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 ViewSpaceDepthPeelNPS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_VIEWSPACEDEPTHPEELN(Default)
PASS_VIEWSPACEDEPTHPEELN(Tess)
PASS_VIEWSPACEDEPTHPEELN(Lines)
PASS_VIEWSPACEDEPTHPEELN(LinesTess)
}

//normalize view space depths z = 1 -> 0 with depth peeling (reversed)
technique11 ViewSpaceDepthPeelNR
{
#define PASS_VIEWSPACEDEPTHPEELNR(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 ViewSpaceDepthPeelNRPS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_VIEWSPACEDEPTHPEELNR(Default)
PASS_VIEWSPACEDEPTHPEELNR(Tess)
PASS_VIEWSPACEDEPTHPEELNR(Lines)
PASS_VIEWSPACEDEPTHPEELNR(LinesTess)
}

//--------techniques for viewing tangents (use in normal renderer)
technique11 Tangents
{
#define PASS_TANGENTS(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 NormalPS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_TANGENTS(Default)
PASS_TANGENTS(Tess)
PASS_TANGENTS(Lines)
PASS_TANGENTS(LinesTess)
}

//--------techniques for Material renderer (use in normal renderer)
technique11 Solid
{
#define PASS_SOLID(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 SolidPS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_SOLID(Default)
PASS_SOLID(Tess)
PASS_SOLID(Lines)
PASS_SOLID(LinesTess)
}

//------Techniques for illumination and shader renderers

technique11 SingleLight
{
#define PASS_SINGLELIGHT(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 singleLightPS( g_lightInfo );\
		HAIR_LIT_HULL_AND_DOMAIN_##PassName\
	}
PASS_SINGLELIGHT(Default)
PASS_SINGLELIGHT(Tess)
PASS_SINGLELIGHT(Lines)
PASS_SINGLELIGHT(LinesTess)
}

technique11 ProjectedLight
{
#define PASS_PROJECTEDLIGHT(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 projLightPS( g_lightInfo, g_projLight, projLightMap );\
		HAIR_LIT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHT(Default)
PASS_PROJECTEDLIGHT(Tess)
PASS_PROJECTEDLIGHT(Lines)
PASS_PROJECTEDLIGHT(LinesTess)
}

technique11 ProjectedLightSuperSample
{
#define PASS_PROJECTEDLIGHTSUPERSAMPLE(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 projLightPS( g_lightInfo, g_projLight, projLightMap );\
		HAIR_LIT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSUPERSAMPLE(Default)
PASS_PROJECTEDLIGHTSUPERSAMPLE(Tess)
PASS_PROJECTEDLIGHTSUPERSAMPLE(Lines)
PASS_PROJECTEDLIGHTSUPERSAMPLE(LinesTess)
}

technique11 ProjectedLightSuperSample2
{
#define PASS_PROJECTEDLIGHTSUPERSAMPLE2(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 projLightPS( g_lightInfo, g_projLight, projLightMap );\
		HAIR_LIT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSUPERSAMPLE2(Default)
PASS_PROJECTEDLIGHTSUPERSAMPLE2(Tess)
PASS_PROJECTEDLIGHTSUPERSAMPLE2(Lines)
PASS_PROJECTEDLIGHTSUPERSAMPLE2(LinesTess)
}

technique11 ProjectedLightSuperSample3
{
#define PASS_PROJECTEDLIGHTSUPERSAMPLE3(PassName)\
	pass P##PassName\
	{\
		PixelShader = compile ps_5_0 projLightPS( g_lightInfo, g_projLight, projLightMap );\
		HAIR_LIT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSUPERSAMPLE3(Default)
PASS_PROJECTEDLIGHTSUPERSAMPLE3(Tess)
PASS_PROJECTEDLIGHTSUPERSAMPLE3(Lines)
PASS_PROJECTEDLIGHTSUPERSAMPLE3(LinesTess)
}
/***************************** eof ***/

