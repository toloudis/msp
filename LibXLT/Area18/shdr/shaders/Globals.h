#version 420

#ifndef _GLOBALS_
#define _GLOBALS_

out gl_PerVertex {
    vec4  gl_Position;
    float gl_ClipDistance[];
};

//This structure is how the data is formatted from the application
//struct STANDARD_VERTEX
//{
	in vec3 InPosition;//	: SV_POSITION;
	in vec3 InNormal;//	: NORMAL;
	in vec2 InUV;//		: TEXCOORD0;
	in vec3 InT;//		: TANGENT;
	in vec3 InB;//		: BINORMAL;
//};

//These structures are used for multipass (single light)
struct TANGENT_MATRIX
{
	vec3 X;// : TEXCOORD3;	//right handed Tangent space basis,
	vec3 Y;// : TEXCOORD4;	// can be non orthogonal,
	vec3 Z;// : TEXCOORD5;	// and vectors are normalized
};

struct TANGENT_VERTEX
{
	vec2 UV;//				: TEXCOORD0;	//original texture coordinates
	vec2 TexCoord0;//		: TEXCOORD1;	//transformed texture coordinates
	vec3 WorldPos;//			: TEXCOORD2;	//world space position
	TANGENT_MATRIX WorldTan;
};

struct TANGENT_VERTEX_OUTPUT
{
	vec4 HPosition;//		: SV_Position;
	TANGENT_VERTEX	V;
	vec3 ScreenPos;//		: TEXCOORD6;	//normalized screen coordinate
};

// data for depth of field
struct DOFvertexOutput 
{
	vec4 HPosition;// : SV_POSITION;
	vec4 ViewSpacePos;// : TEXCOORD0;
	vec2 TexCoord0;//	: TEXCOORD1;	//transformed texture coordinates
};

mat4 g_uvTransform = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

//User defined clipping plane (world space defined)
vec4 g_ClipPlane = vec4( 0, 0, 0, 1);	//set to no clipping

float ClipWorldPos( in vec3 WorldPos )
{
	return dot( vec4(WorldPos,1), g_ClipPlane );
}

// uv * xy + zw
vec4 g_bakeTransform = vec4(1,1,0,0);
// should the vtx shaders run in bake mode or standard mode?
bool g_bake = false;
// convert vertex uv coordinate to clip space position for texture baking.
vec4 BakeVertex(in vec2 i_UV, in mat4 wvp)
{
	// this step assures that the baked texture captures 
	// the entire texture space of a mesh that has pre-tiled uvs.
	vec2 untiledUV = clamp(i_UV * g_bakeTransform.xy + g_bakeTransform.zw, vec2(0,0), vec2(1,1));
	untiledUV.y = 1.0f - untiledUV.y;

	vec4 o_hPos;

//	o_hPos.xy = untiledUV*2 - vec2(1,1);
//	o_hPos.y = -o_hPos.y;
//	o_hPos.zw = i_UV.zw;

	//untiledUV = lerp(vec2(-0.005f, -0.005f), vec2(1.005f, 1.005f), untiledUV);
	untiledUV -= 0.5;
	untiledUV *= 2.0f;
	untiledUV = clamp(untiledUV.xy, -1.0f, 1.0f);
	
	o_hPos = vec4(untiledUV,0.5,1);
	//o_hPos = mul(wvp, vec4(untiledUV,0.5,1));


	return o_hPos;
}

// decide how to best get the vertex to clip space, and then do it!
vec4 TransformVertex(in vec4 i_Po, in vec2 i_UV, in mat4 wvp)
{
	vec4 o_hPos;
    if (g_bake)
		o_hPos = BakeVertex(i_UV, wvp);
    else
		o_hPos = mul(wvp, i_Po);
	return o_hPos;
}

#endif//_GLOBALS_
