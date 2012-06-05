/*****************************************************************************
**  Globals.h
**
**      Global variables and constant that are shared by all shaders
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifndef _GLOBALS_
#define _GLOBALS_

//This structure is how the data is formatted from the application
struct STANDARD_VERTEX
{
	float3 Position	: SV_POSITION;
	float3 Normal	: NORMAL;
	float2 UV		: TEXCOORD0;
	float3 T		: TANGENT;
	float3 B		: BINORMAL;
};

//These structures are used for multipass (single light)
struct TANGENT_MATRIX
{
	float3 X : TEXCOORD3;	//right handed Tangent space basis,
	float3 Y : TEXCOORD4;	// can be non orthogonal,
	float3 Z : TEXCOORD5;	// and vectors are normalized
};

struct TANGENT_VERTEX
{
	float2 UV				: TEXCOORD0;	//original texture coordinates
	float2 TexCoord0		: TEXCOORD1;	//transformed texture coordinates
	float3 WorldPos			: TEXCOORD2;	//world space position
	TANGENT_MATRIX WorldTan;
};

struct TANGENT_VERTEX_OUTPUT
{
	float4 HPosition		: SV_Position;
	TANGENT_VERTEX	V;
	float3 ScreenPos		: TEXCOORD6;	//normalized screen coordinate
};

// data for depth of field
struct DOFvertexOutput 
{
	float4 HPosition : SV_POSITION;
	float4 ViewSpacePos : TEXCOORD0;
	float2 TexCoord0	: TEXCOORD1;	//transformed texture coordinates
};

float4x4 g_uvTransform = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

//User defined clipping plane (world space defined)
float4 g_ClipPlane = float4( 0, 0, 0, 1);	//set to no clipping

float ClipWorldPos( in float3 WorldPos )
{
	return dot( float4(WorldPos,1), g_ClipPlane );
}

// uv * xy + zw
float4 g_bakeTransform = float4(1,1,0,0);
// should the vtx shaders run in bake mode or standard mode?
bool g_bake = false;
// convert vertex uv coordinate to clip space position for texture baking.
float4 BakeVertex(in float2 i_UV, in float4x4 wvp)
{
	// this step assures that the baked texture captures 
	// the entire texture space of a mesh that has pre-tiled uvs.
	float2 untiledUV = saturate(i_UV * g_bakeTransform.xy + g_bakeTransform.zw);
	untiledUV.y = 1.0f - untiledUV.y;

	float4 o_hPos;

//	o_hPos.xy = untiledUV*2 - float2(1,1);
//	o_hPos.y = -o_hPos.y;
//	o_hPos.zw = i_UV.zw;

	//untiledUV = lerp(float2(-0.005f, -0.005f), float2(1.005f, 1.005f), untiledUV);
	untiledUV -= 0.5;
	untiledUV *= 2.0f;
	untiledUV = clamp(untiledUV.xy, -1.0f, 1.0f);
	
	o_hPos = float4(untiledUV,0.5,1);
	//o_hPos = mul(wvp, float4(untiledUV,0.5,1));


	return o_hPos;
}

// decide how to best get the vertex to clip space, and then do it!
float4 TransformVertex(in float4 i_Po, in float2 i_UV, in float4x4 wvp)
{
	float4 o_hPos;
    if (g_bake)
		o_hPos = BakeVertex(i_UV, wvp);
    else
		o_hPos = mul(wvp, i_Po);
	return o_hPos;
}

#endif//_GLOBALS_
