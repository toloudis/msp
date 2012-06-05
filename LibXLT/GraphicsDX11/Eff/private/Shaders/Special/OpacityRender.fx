/*****************************************************************************
**  OpacityRender.fx
**
**  Renders out Opacity Maps
**
**  Projected:    Render opacity in pixel shader.
**  View:  Renders opacities for actual depths.
**
**  Deep versions use input depth buffer.
**
**  Hair versions to handle hair objects
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

#include "..\Support.h"
#include "..\SupportHair.h"

bool hasDiffuseMap = false;
Texture2D diffuseMap;

float g_Transparency = 1.0f;

//const float alphaThreshold = 254.0f/255.0f;

/************* DATA STRUCTS **************/

/* data passed from vertex shader to pixel shader */
struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
    float  Depth		: TEXCOORD0; //Linear view space depth
	float  Opacity      : TEXCOORD1;
    float2 texCoord		: TEXCOORD2; //model UV's
};

struct GeometryOutput
{
    float4 HPosition	: SV_POSITION;
    float  Depth		: TEXCOORD0; //Linear view space depth
	float  Opacity      : TEXCOORD1;
    float2 texCoord		: TEXCOORD2; //model UV's
	float  ClipDist     : SV_ClipDistance0;
};

struct VertexOutputHairNoTess
{
    float4 HPosition	: SV_POSITION;
    float  Depth		: TEXCOORD0; //Linear view space depth
	float  Opacity      : TEXCOORD1;
    float2 texCoord		: TEXCOORD2; //model UV's
	nointerpolation uint Segment	: TEXCOORD3;	//use to cull out degenerate strands
};

struct VertexOutputHair
{
    float4 HPosition	: SV_POSITION;
    float4 texCoord		: TEXCOORD0;
	float4 TanOpacity	: COLOR0;		//pack tangent and opacity
	nointerpolation uint Segment	: TEXCOORD2;	//use to cull out degenerate strands
};

struct VertexOutputTess
{
    float2 texCoord		: TEXCOORD0; //model UV's
    float3 Position		: TEXCOORD1;
	float3 Normal		: TEXCOORD2; //world space normal
};

struct pixelOutput4
{
	float4 ChannelA : SV_Target0;
	float4 ChannelB : SV_Target1;
	float4 ChannelC : SV_Target2;
	float4 ChannelD : SV_Target3;
};

struct pixelOutput8
{
	float4 ChannelA : SV_Target0;
	float4 ChannelB : SV_Target1;
	float4 ChannelC : SV_Target2;
	float4 ChannelD : SV_Target3;
	float4 ChannelE : SV_Target4;
	float4 ChannelF : SV_Target5;
	float4 ChannelG : SV_Target6;
	float4 ChannelH : SV_Target7;
};

//-----------------------------------------------------------------------------
// Vertex Shaders
//-----------------------------------------------------------------------------
VertexOutput DepthVS_Default( STANDARD_VERTEX IN, out float ClipDist : SV_ClipDistance0 )
{
    VertexOutput OUT;

	float4 Po = float4(IN.Position, 1.0f);
    OUT.HPosition = mul( g_wvp, Po );
    OUT.Depth = mul( g_wv, Po ).z;
    OUT.texCoord = mul(g_uvTransform, float4(IN.UV,0,1)).xy;
	OUT.Opacity = g_Transparency;
    
	float3 Pw = mul( g_world, Po ).xyz;
	ClipDist = ClipWorldPos( Pw );

    return OUT;
}

VertexOutputTess DepthVS_Tess( STANDARD_VERTEX IN )
{
    VertexOutputTess OUT;

	float4 Po = float4(IN.Position, 1.0f);
    OUT.texCoord = mul(g_uvTransform, float4(IN.UV,0,1)).xy;
    OUT.Position = mul( g_world, Po ).xyz;
	OUT.Normal = mul( (float3x3)g_world, IN.Normal );
    
    return OUT;
}

//-----------Vertex shader for opacity shadow maps-----------
VertexOutputHairNoTess HairDepthVS_NoTess( uint VertID : SV_VertexID )
{
    VertexOutputHairNoTess OUT;

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

	OUT.Opacity = g_Transparency * Mtl.m_Opacity;
	OUT.Depth = mul( g_wv, float4( Pos, 1)).z;

	float radius = lerp( Mtl.m_RootRadius, Mtl.m_TipRadius, t );

	OUT.HPosition = float4( vWorldPos, abs(radius) );
	OUT.texCoord = float2( step( 0, radius ), t );
	OUT.Segment = curSegment;

    return OUT;
}

//NULL vertex shader
void HairDepthVS_Tess(){}

//-----------------------------------------------------------------------------
// Pixel Shaders
//-----------------------------------------------------------------------------

SamplerState diffuseSampler
{
	FILTER = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = WRAP;
};

//renders out 4 layers of opacity between Depth and ZFar into the RGBA color channels
//if a depth map is valid then start will offset for deep OSM
pixelOutput OpacityShadow4PS( VertexOutput In )
{   
	pixelOutput Out = (pixelOutput)0;

	float alpha = In.Opacity;
	float depth = In.Depth;   //actual depth

	float DZ = (g_ZFar - g_ZNear) / 4;
	float d = g_ZNear;
	if( g_HasDepthMap )
	{
		//half texel offset and buffer scaling
		float2 UV = In.HPosition.xy * g_InvScreenSize;

		d = (depthMap.Sample( HairSampler, UV ).r * (g_ZFar - g_ZNear)) + g_ZNear;	//rescale to actual depth;
	}
	float4 D = {d+DZ*1, d+DZ*2, d+DZ*3, d+DZ*4};

	float4 V = (D - depth) / DZ;
	Out.col = step( 0, V ) * step( V, 1 ) * alpha;
	return Out;
}

//renders out 16 layers of opacity between ZNear and ZFar into the RGBA color channels of the 4 targets
//if a depth map is valid then start will offset for deep OSM
pixelOutput4 OpacityShadow16PS( VertexOutput In )
{   
	pixelOutput4 Out = (pixelOutput4)0;

	float alpha = In.Opacity;
	float depth = In.Depth;   //actual depth

	float DZ = (g_ZFar - g_ZNear) / 16;
	float d = g_ZNear;
	if( g_HasDepthMap )
	{
		//half texel offset and buffer scaling
		float2 UV = In.HPosition.xy * g_InvScreenSize;

		d = (depthMap.Sample( HairSampler, UV ).r * (g_ZFar - g_ZNear)) + g_ZNear;	//rescale to actual depth;
	}

	float4 DA = {d+DZ*1, d+DZ*2, d+DZ*3, d+DZ*4};
	float4 DB = {d+DZ*5, d+DZ*6, d+DZ*7, d+DZ*8};
	float4 DC = {d+DZ*9, d+DZ*10, d+DZ*11, d+DZ*12};
	float4 DD = {d+DZ*13, d+DZ*14, d+DZ*15, d+DZ*16};

	float4 VA = (DA - depth) / DZ;
	float4 VB = (DB - depth) / DZ;
	float4 VC = (DC - depth) / DZ;
	float4 VD = (DD - depth) / DZ;
	Out.ChannelA = step( 0, VA ) * step( VA, 1 ) * alpha;
	Out.ChannelB = step( 0, VB ) * step( VB, 1 ) * alpha;
	Out.ChannelC = step( 0, VC ) * step( VC, 1 ) * alpha;
	Out.ChannelD = step( 0, VD ) * step( VD, 1 ) * alpha;
	return Out;
}

//renders out 32 layers of opacity between ZNear and ZFar into the RGBA color channels of the 8 targets
//if a depth map is valid then start will offset for deep OSM
pixelOutput8 OpacityShadow32PS( VertexOutput In )
{   
	pixelOutput8 Out = (pixelOutput8)0;

	float alpha = In.Opacity;
	float depth = In.Depth;   //actual depth

	float DZ = (g_ZFar - g_ZNear) / 32;
	float d = g_ZNear;
	if( g_HasDepthMap )
	{
		//half texel offset and buffer scaling
		float2 UV = In.HPosition.xy * g_InvScreenSize;

		d = (depthMap.Sample( HairSampler, UV ).r * (g_ZFar - g_ZNear)) + g_ZNear;	//rescale to actual depth;
	}

	float4 DA = {d+DZ*1, d+DZ*2, d+DZ*3, d+DZ*4};
	float4 DB = {d+DZ*5, d+DZ*6, d+DZ*7, d+DZ*8};
	float4 DC = {d+DZ*9, d+DZ*10, d+DZ*11, d+DZ*12};
	float4 DD = {d+DZ*13, d+DZ*14, d+DZ*15, d+DZ*16};
	float4 DE = {d+DZ*17, d+DZ*18, d+DZ*17, d+DZ*20};
	float4 DF = {d+DZ*21, d+DZ*22, d+DZ*23, d+DZ*24};
	float4 DG = {d+DZ*25, d+DZ*26, d+DZ*27, d+DZ*28};
	float4 DH = {d+DZ*29, d+DZ*30, d+DZ*31, d+DZ*32};

	float4 VA = (DA - depth) / DZ;
	float4 VB = (DB - depth) / DZ;
	float4 VC = (DC - depth) / DZ;
	float4 VD = (DD - depth) / DZ;
	float4 VE = (DE - depth) / DZ;
	float4 VF = (DF - depth) / DZ;
	float4 VG = (DG - depth) / DZ;
	float4 VH = (DH - depth) / DZ;
	Out.ChannelA = step( 0, VA ) * step( VA, 1 ) * alpha;
	Out.ChannelB = step( 0, VB ) * step( VB, 1 ) * alpha;
	Out.ChannelC = step( 0, VC ) * step( VC, 1 ) * alpha;
	Out.ChannelD = step( 0, VD ) * step( VD, 1 ) * alpha;
	Out.ChannelE = step( 0, VE ) * step( VE, 1 ) * alpha;
	Out.ChannelF = step( 0, VF ) * step( VF, 1 ) * alpha;
	Out.ChannelG = step( 0, VG ) * step( VG, 1 ) * alpha;
	Out.ChannelH = step( 0, VH ) * step( VH, 1 ) * alpha;
	return Out;
}

void OpacityShadowVolumePS( VertexOutput In )
{
	uint3 dims;

//	OpacityShadowBuffer3D.GetDimensions( dims.x, dims.y, dims.z );
	uint3 icoord;
	icoord.xy = In.HPosition.xy;
	icoord.z = 0;

	OpacityShadowBuffer3D[ icoord ] = In.Opacity;
}

//--------------------------------------------------------------------------------------
// Hull shader
//--------------------------------------------------------------------------------------

HS_CONSTANT_DATA_OUTPUT Opacity_Constants_HS( InputPatch<VertexOutputTess, 3> inputPatch )
{
	return AdaptiveTessellate(inputPatch[0].Position,
		inputPatch[1].Position,
		inputPatch[2].Position);
}

[domain("tri")]
[partitioning(SGPU_PARTITIONING)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("Opacity_Constants_HS")]
VertexOutputTess Opacity_HS( InputPatch<VertexOutputTess, 3> inputPatch, uint uCPID : SV_OutputControlPointID )
{
	return inputPatch[uCPID];
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("tri")]
VertexOutput Opacity_DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
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

    output.texCoord = TexCoord0;

	//need normal to displace
#ifdef APPLY_DISPLACEMENT
	vWorldPos = DisplaceVertex( vWorldPos, vNormal, TexCoord0 );
#endif

	output.Depth = mul( g_view, float4( vWorldPos, 1.0f )).z;

	// Transform world position with viewprojection matrix
	output.HPosition = mul( g_vp, float4( vWorldPos, 1.0f ) );

	ClipDist = ClipWorldPos( vWorldPos );

	return output;
}

#define OPACITY_HULL_AND_DOMAIN_Default

#define OPACITY_HULL_AND_DOMAIN_Tess\
	SetHullShader(CompileShader(hs_5_0, Opacity_HS()));\
	SetDomainShader(CompileShader(ds_5_0, Opacity_DS()));


//-------------------------Hair support shaders-----------------------------------------

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("isoline")]
VertexOutputHair Hair_DS( HS_HAIR_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, 
					 const OutputPatch<hairOutput, 4> CP, uint PatchID : SV_PrimitiveID )
{
	VertexOutputHair output = (VertexOutputHair)0;

	float3 vWorldPos;
	float3 vWorldTan;

	float t = InterpolateHairStrand( CP, PatchID, UV, vWorldPos, vWorldTan );
	hairMaterial Mtl = GetMaterial( PatchID );

	float radius = lerp( Mtl.m_RootRadius, Mtl.m_TipRadius, t );
	output.HPosition = float4( vWorldPos, abs(radius) );
	output.TanOpacity = float4( vWorldTan, g_Transparency * Mtl.m_Opacity );
	output.texCoord = float4( step( 0, radius ), t, 0, 0 );
	output.Segment = PatchID % g_nStrandCPs;

	return output;
}

[domain("isoline")]
VertexOutput HairLines_DS( HS_HAIR_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, 
					 const OutputPatch<hairOutput, 4> CP, uint PatchID : SV_PrimitiveID )
{
	VertexOutput output = (VertexOutput)0;

	float3 vWorldPos;
	float3 vWorldTan;

	float t = InterpolateHairStrand( CP, PatchID, UV, vWorldPos, vWorldTan );
	hairMaterial Mtl = GetMaterial( PatchID );

	float radius = lerp( Mtl.m_RootRadius, Mtl.m_TipRadius, t );
	output.Opacity = g_Transparency * Mtl.m_Opacity;
	output.HPosition = mul( g_vp, float4( vWorldPos, 1) );
	output.Depth =  mul( g_view, float4( vWorldPos, 1) ).z;
	output.texCoord = float2( step( 0, radius ), t );

	return output;
}


//--------------------------------------------------------------------------------------
// Geometry Shader
//--------------------------------------------------------------------------------------
[maxvertexcount(4)]
void Hair_GS( line VertexOutputHair input[2], inout TriangleStream<GeometryOutput> TriStream )
{
	GeometryOutput output;

	if( input[0].Segment >= (uint)(g_nStrandCPs-1) ) return;	//cull out degenerate segments

	for( uint i = 0; i < 2; i++ )
	{
		float radius = input[i].HPosition.w;
		float3 newPos = input[i].HPosition.xyz;
		float3 WTan = input[i].TanOpacity.xyz;

		float3 Offset;
		float SubPixel = HairLimitExpand( newPos, WTan, radius, Offset );

		float3 WPos = newPos + (Offset * radius);	//position.w is signed radius
		output.HPosition = mul( g_vp, float4( WPos, 1) );
	    output.Depth = mul( g_view, float4(WPos,1) ).z;
		output.texCoord = input[i].texCoord.xy;
		output.Opacity = input[i].TanOpacity.w * SubPixel;
		output.ClipDist = ClipWorldPos( WPos );
		TriStream.Append( output );

		WPos = newPos + (Offset * -radius);	//position.w is signed radius
		output.HPosition = mul( g_vp, float4( WPos, 1) );
	    output.Depth = mul( g_view, float4(WPos,1) ).z;
		output.ClipDist = ClipWorldPos( WPos );
		TriStream.Append( output );
	}

    TriStream.RestartStrip();
}

[maxvertexcount(2)]
void HairLines_GS( line VertexOutputHairNoTess input[2], inout LineStream<GeometryOutput> Lines )
{
	GeometryOutput output;

	if( input[0].Segment >= (uint)(g_nStrandCPs-1) ) return;	//cull out degenerate segments

	for( uint i = 0; i < 2; i++ )
	{
		float3 WPos = input[i].HPosition.xyz;

		output.HPosition = mul( g_vp, float4( WPos, 1) );
	    output.Depth = mul( g_view, float4(WPos,1) ).z;
		output.texCoord = input[i].texCoord;
		output.Opacity = input[i].Opacity.x;
		output.ClipDist = ClipWorldPos( WPos );
		Lines.Append( output );
	}

    Lines.RestartStrip();
}


#define HAIR_HULL_AND_DOMAIN_Default\
	SetVertexShader(CompileShader(vs_5_0, HairDepthVS_NoTess()));\

#define HAIR_HULL_AND_DOMAIN_Tess\
	SetVertexShader(CompileShader(vs_5_0, HairDepthVS_Tess()));\
	SetHullShader(CompileShader(hs_5_0, Hair_HS()));\
	SetDomainShader(CompileShader(ds_5_0, Hair_DS()));\
	SetGeometryShader(CompileShader(gs_5_0, Hair_GS()));

#define HAIR_HULL_AND_DOMAIN_Lines\
	SetVertexShader(CompileShader( vs_5_0, HairDepthVS_NoTess()));\
	SetGeometryShader(CompileShader(gs_5_0, HairLines_GS()));

#define HAIR_HULL_AND_DOMAIN_LinesTess\
	SetVertexShader(CompileShader( vs_5_0, HairDepthVS_Tess()));\
	SetHullShader(CompileShader(hs_5_0, Hair_HS()));\
	SetDomainShader(CompileShader(ds_5_0, HairLines_DS()));\

//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------
technique11 OpacityShadowMap4
{
#define PASS_OPACITYSHADOWMAP(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 DepthVS_##PassName();\
		PixelShader = compile ps_5_0 OpacityShadow4PS();\
		OPACITY_HULL_AND_DOMAIN_##PassName\
	}
PASS_OPACITYSHADOWMAP(Default)
PASS_OPACITYSHADOWMAP(Tess)
}

technique11 OpacityShadowMap16
{
#define PASS_OPACITYSHADOWMAP16(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 DepthVS_##PassName();\
		PixelShader = compile ps_5_0 OpacityShadow16PS();\
		OPACITY_HULL_AND_DOMAIN_##PassName\
	}
PASS_OPACITYSHADOWMAP16(Default)
PASS_OPACITYSHADOWMAP16(Tess)
}

technique11 OpacityShadowMap32
{
#define PASS_OPACITYSHADOWMAP32(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 DepthVS_##PassName();\
		PixelShader = compile ps_5_0 OpacityShadow32PS();\
		OPACITY_HULL_AND_DOMAIN_##PassName\
	}
PASS_OPACITYSHADOWMAP32(Default)
PASS_OPACITYSHADOWMAP32(Tess)
}

technique11 OpacityShadowVolume
{
#define PASS_OPACITYSHADOWVOLUME(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 DepthVS_##PassName();\
		PixelShader = compile ps_5_0 OpacityShadowVolumePS();\
		OPACITY_HULL_AND_DOMAIN_##PassName\
	}
PASS_OPACITYSHADOWVOLUME(Default)
PASS_OPACITYSHADOWVOLUME(Tess)
}

//-----------just for hair------------------
technique11 HairOpacityShadowMap4
{
#define PASS_HAIROPACITYSHADOWMAP(PassName)	\
	pass P##PassName			\
	{						\
		PixelShader = compile ps_5_0 OpacityShadow4PS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_HAIROPACITYSHADOWMAP(Default)
PASS_HAIROPACITYSHADOWMAP(Tess)
PASS_HAIROPACITYSHADOWMAP(Lines)
PASS_HAIROPACITYSHADOWMAP(LinesTess)
}

technique11 HairOpacityShadowMap16
{
#define PASS_HAIROPACITYSHADOWMAP16(PassName)	\
	pass P##PassName			\
	{						\
		PixelShader = compile ps_5_0 OpacityShadow16PS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_HAIROPACITYSHADOWMAP16(Default)
PASS_HAIROPACITYSHADOWMAP16(Tess)
PASS_HAIROPACITYSHADOWMAP16(Lines)
PASS_HAIROPACITYSHADOWMAP16(LinesTess)
}

technique11 HairOpacityShadowMap32
{
#define PASS_HAIROPACITYSHADOWMAP32(PassName)	\
	pass P##PassName			\
	{						\
		PixelShader = compile ps_5_0 OpacityShadow32PS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_HAIROPACITYSHADOWMAP32(Default)
PASS_HAIROPACITYSHADOWMAP32(Tess)
PASS_HAIROPACITYSHADOWMAP32(Lines)
PASS_HAIROPACITYSHADOWMAP32(LinesTess)
}

technique11 HairOpacityShadowVolume
{
#define PASS_HAIROPACITYSHADOWVOLUME(PassName)	\
	pass P##PassName			\
	{						\
		PixelShader = compile ps_5_0 OpacityShadowVolumePS();\
		HAIR_HULL_AND_DOMAIN_##PassName\
	}
PASS_HAIROPACITYSHADOWVOLUME(Default)
PASS_HAIROPACITYSHADOWVOLUME(Tess)
PASS_HAIROPACITYSHADOWVOLUME(Lines)
PASS_HAIROPACITYSHADOWVOLUME(LinesTess)
}

/***************************** eof ***/
