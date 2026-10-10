//////////////////////////////////////////////////////////////////////////////
// Converted from Globals.h by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// This file is the source of truth for the material shaders from here on;
// the original Globals.h was deleted along with the Effects (.fx) shaders.
//
// Binding model shared by ALL material shaders (plain HLSL, SM5 and SM6).
// Every resource has an explicit register so the layout is identical for
// every entry point of every material, and maps directly onto a D3D12 root
// signature / Vulkan descriptor set laid out by update rate.
//
//   Constant buffers (declared once, here)
//     b0  FrameParams     per frame / view
//     b1  ObjectParams    per draw / object
//     b2  LightParams     per light (pass)
//     b3  MaterialCommon  material inputs declared by the shared includes
//     b4  MaterialParams  the material's own parameters (declared by each
//                         material .hlsl, never by a shared include)
//
//   Textures
//     t0-t19   material textures
//                t0 normalMap          (Support.hlsli)
//                t1 diffuseEnvMap      (Support.hlsli)
//                t2 specularEnvMap     (Support.hlsli)
//                t3 g_DisplacementMap  (Tessellate.hlsli)
//                t4-t19 the material's own textures
//     t20+     renderer-provided textures
//                t20 g_cubeMap         (reflection cube map, declared by
//                t21 g_planarMap        the reflective materials)
//                t22 projLightMap      (Lighting.hlsli)
//                t23 projShadowMap     (Lighting.hlsli)
//                t24 g_Poisson         (Lighting.hlsli)
//                t25 meshDataTexture   (Tessellate.hlsli)
//
//   Samplers (immutable; states are described in each .effect.json)
//     s0-s9    shared
//                s0 g_DefaultSampler    s1 g_CubeSampler      (Support.hlsli)
//                s2 bumpMapSampler      s3 displacementSampler
//                s4 displacementTapSampler                    (Tessellate.hlsli)
//                s5 projSampler         s6 shadowMapSampler
//                s7 shdwSampler         s8 SATSampler
//                s9 CubeSampler                               (Lighting.hlsli)
//     s10-s15  the material's own samplers
//
// Defaults of the cbuffer members (the initializers the .fx had) live in the
// "variables" section of each .effect.json; plain HLSL ignores initializers
// on constants. The original Effects semantic of each member is noted in a
// comment and recorded in the manifest, which is what the runtime reads.
//
// C++ uploads the LightInfo / ProjLightInfo structs with memcpy of C++
// structs that match the Effects packing; plain cbuffer packing is the same,
// so do not reorder or pad their members.
//////////////////////////////////////////////////////////////////////////////

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

/*********** light structures (moved here from Lighting.h for LightParams) ******/
struct LightInfo
{
	float4 Pos;
	float4 Diffuse;
	float4 Specular;
	float4 Falloff;
	float4 ConeInfo;	/* x,y,z are normalized direction, w is cos(ConeAngle) */
	float FalloffStart;
};
struct ProjLightInfo
{
	float4 Pos;
	float4x4 Matrix;
	float LightSize;
	float PCSSAdjust;
	float Scale;
	float ShadowIntensity;
	float4 ShadowColor;
	float2 NearFar;
	float MapSize;
	float InnerAngle;
	float OuterAngle;
	float Aspect;
};

// Size of the skinning matrix palette. Part of the ObjectParams layout, so it
// is fixed for every material (Skinning.h used to allow overriding it).
#define MATRIX_PALETTE_SIZE_DEFAULT 50

//////////////////////////////////////////////////////////////////////////////
// b0: per frame / view
//////////////////////////////////////////////////////////////////////////////
cbuffer FrameParams : register(b0)
{
	float4x4 g_vp;					// : ViewProjection
	float4x4 g_proj;				// : Projection
	float4x4 g_view;				// : View
	float4 g_eyePos;				// : CameraPos
	// w, h, 1/w, 1/h
	float4 g_targetRes;
	// g_vDofParams
	// x = near blur depth
	// y = near focal plane depth
	// z = far focal plane depth
	// w = far blur depth
	float4 g_vDofParams;			// default (30, 60, 80, 120)
	// blurriness cutoff constant for objects behind focal plane
	//		(1.0 = max blur, 0.0 = max sharpness)
	float g_fDofBlurCutoff;			// default 1
	// time for shader animation
	float g_time_0_X;
	bool g_IsReflectionGen;			// : IsReflectionGen, default false
	// because reflections are folded into the env pass,
	// this flag tells the env pass to render the reflection contrib only.
	bool g_IsolateReflection;		// default false
	// This flag trun on/off reflection effect
	bool g_bCubeMapEnabled;			// default false
};

//////////////////////////////////////////////////////////////////////////////
// b1: per draw / object
//////////////////////////////////////////////////////////////////////////////
cbuffer ObjectParams : register(b1)
{
	float4x4 g_world;				// : World
	float4x4 g_worldIT;				// : WorldIT
	float4x4 g_wv;					// : WorldView
	float4x4 g_wvp;					// : WorldViewProjection
	float4x4 g_uvTransform;			// default identity
	//User defined clipping plane (world space defined)
	float4 g_ClipPlane;				// default (0,0,0,1): no clipping
	// uv * xy + zw
	float4 g_bakeTransform;			// default (1,1,0,0)
	//tessellation factors
	// x:edge, y:inside, z:screen space tri edge size limit (in pixels)
	float4 g_vTessellationFactor;
	// displacement mapping
	float2 g_DisplacementMapSize;
	float2 g_ObjectUVScale;
	float g_DisplacementScale;
	float g_DisplacementBias;
	float g_DisplacementBlur;
	bool g_hasDisplacementMap;		// default false
	// size of meshDataTexture
	float g_MeshDataTextureWidth;	// default 256 (make sure this is a multiple of 16)
	float g_MeshDataTextureHeight;	// default 64
	// should the vtx shaders run in bake mode or standard mode?
	bool g_bake;					// default false
	// skinning palette, last so the rest of the buffer stays compact
	float4x4 g_BonePalette[MATRIX_PALETTE_SIZE_DEFAULT];	// : BONES
};

//////////////////////////////////////////////////////////////////////////////
// b2: per light
//////////////////////////////////////////////////////////////////////////////
cbuffer LightParams : register(b2)
{
	LightInfo g_lightInfo;			// : LightInfo
	ProjLightInfo g_projLight;		// : ProjLightInfo
	LightInfo g_lightArray[8];		// : LightArray
	int g_lightArrayNum;			// : LightArrayNum, default 0
	bool g_FirstLight;				// default false: when true env pass is combined with a lit pass
	bool g_bProjLt;					// default false
	bool g_bHasProjMap;				// : HasProjectedTexture, default false
	bool g_bHasShadowMap;			// : HasShadowMap, default false
	float g_DistributeFactor;		// default 256 (VSM precision split)
};

//////////////////////////////////////////////////////////////////////////////
// b3: material inputs declared by the shared includes
//////////////////////////////////////////////////////////////////////////////
cbuffer MaterialCommon : register(b3)
{
	// used for ambient environment image-based lighting
	float4 g_envDiffuseColor;		// default (1,1,1,1)
	float4 g_envSpecularColor;		// default (1,1,1,1)
	float g_diffuseFactor;			// default 0.1
	bool g_bHasDiffuseEnvMap;
	float g_diffuseEnvAngle;		// default 0
	// FC3D doesn't know about specular environments (kept in the layout anyway)
	float g_specularFactor;			// default 0.1
	bool g_bHasSpecularEnvMap;
	float g_specularEnvAngle;		// default 0
	float g_bumpMapScale;			// : BumpMapScale, default 0
	bool hasNormalMap;				// default false
	//alpha testing reference value (always set to Greater Equal comparison)
	float g_AlphaTestRef;			// default 0
	// material properties
	bool g_bDoubleSided;			// : DoubleSided, default false
	// FC3D doesn't know about glow (kept in the layout anyway)
	// amount to extrude geometry for glow effect
	float g_glowSize;				// default 0
	bool g_bHasMask;				// default false
	bool g_bConstGlow;				// default false
	//if this flag is defined in a shader then hardware tessellation shaders are supported
	bool hasHardwareTessellation;	// default true
};

float ClipWorldPos( in float3 WorldPos )
{
	return dot( float4(WorldPos,1), g_ClipPlane );
}

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
