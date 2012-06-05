/*****************************************************************************
/* HEADER 
/****************************************************************************/
int gp : SasGlobal
<
 int3 SasVersion = {1,0,0};
 string SasEffectAuthor   = "Daniel Toloudis";
 string SasEffectAuthoringSoftware = "MachStudio";
 string SasEffectCategory   = "/material";
 string SasEffectCompany   = "studio|gpu";
 string SasEffectDescription  = "Phong w/Bump";
 string SasEffectHelp    = "This is a Phong shader."; 
 string SasEffectRevision   = "2"; 
>;

float Os : Opacity = 1.0f; 
float4 Csd : MaterialDiffuse = {1.0f, 1.0f, 1.0f, 1.0f};
float4 Css : MaterialSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
float g_bumpMapScale : BumpMapScale = 1.0f;

// Normal
bool hasNormalMap = false;  // default, ibl
texture2D normalMap : NormalMap;
sampler2D normalSampler = sampler_state // all PS's except dofprep & matte
{
 Texture = <normalMap>;
 MinFilter = Linear;
 MagFilter = Linear;
 MipFilter = Linear;
 AddressU = WRAP;
 AddressV = WRAP;
};

// Transparency
bool hasTransparencyMap = false; // default, ibl
texture2D transparencyMap : OpacityTexture;
sampler2D transparencySampler = sampler_state // default, ibl
{
 Texture = <transparencyMap>;
 MinFilter = Linear;
 MagFilter = Linear;
 MipFilter = Linear;
 AddressU = WRAP;
 AddressV = WRAP;
};

// Glow
texture2D glowMask  : GlowMask;
sampler2D glowSampler = sampler_state // glow
{
 Texture = <glowMask>;
 MinFilter = Linear;
 MagFilter = Linear;
 MipFilter = Linear;
};
/*****************************************************************************
** Globals.h
**
** Global variables and constant that are shared by all shaders
**
** Studio GPU
** Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifndef _GLOBALS_
#define _GLOBALS_

struct STANDARD_VERTEX {
 float3 Position : POSITION;
 float3 Normal : NORMAL;
 float4 UV  : TEXCOORD0;
 float3 T  : TANGENT;
 float3 B  : BINORMAL;
};

struct TANGENT_MATRIX
{
 float3 X : TEXCOORD3; //right handed Tangent space basis,
 float3 Y : TEXCOORD4; // can be non orthogonal,
 float3 Z : TEXCOORD5; // and vectors are normalized
};

struct TANGENT_VERTEX
{
 float4 UV    : TEXCOORD0; //original texture coordinates
 float4 TexCoord0  : TEXCOORD1; //transformed texture coordinates
 float3 WorldPos   : TEXCOORD2; //world space position
 TANGENT_MATRIX WorldTan;
};

struct TANGENT_VERTEX_OUTPUT
{
 float4 HPosition  : POSITION;
 TANGENT_VERTEX V;
 float3 ScreenPos  : TEXCOORD6; //normalized screen coordinate
};

struct TANGENT_CLR_VERTEX_OUTPUT
{
 float4 HPosition  : POSITION;
// TANGENT_VERTEX V;
 float4 TexCoord0  : TEXCOORD0; //transformed texture coordinates
 float4 UV    : TEXCOORD1; //original texture coordinates
 float3 WorldEyeDir  : TEXCOORD2; //world space eye direction
 TANGENT_MATRIX WorldTan;
// float3 ScreenPos  : TEXCOORD6; //normalized screen coordinate
 float4 diffCol   : COLOR0;  //combined diffuse color
 float4 specCol   : COLOR1;  //combined specular color
};

float4x4 g_uvTransform = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

#endif//_GLOBALS_
/*****************************************************************************
** Support.h
**
** Support functions for .fx functions
**
** Gigawatt Studios
** Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifndef _GLOBALS_
#include "Globals.h"
#endif

#define PRE_MULT_ALPHA  //define to have alpha pre multiplied by RGB, alpha still pass on.

//constants
const float HALFPI = 1.57079633;
const float PI = 3.14159265;
const float TWOPI = 6.28318531;
const float INVHALFPI = 0.636619772;//1.0f/1.57079633;
const float INVPI = 0.318309886;//1.0f/3.14159265;
const float INVTWOPI = 0.159154943;//1.0f/6.28318531;
const float PI_DIV_180 = 0.0174532925;//PI/180

// transforms/viewing
float4x4 g_worldIT : WorldIT;
float4x4 g_wvp : WorldViewProjection;
float4x4 g_wv : WorldView;
float4x4 g_wvIT : WorldViewIT;
float4x4 g_world : World;
float4x4 g_viewIT : ViewIT;
float4x4 g_view : View;
float4x4 g_vp : ViewProjection;
float4 g_eyePos : CameraPos;

// should the vtx shaders run in bake mode or standard mode?
bool g_bake = false;
// uv * xy + zw
float4 g_bakeTransform = float4(1,1,0,0);

// time for shader animation
float g_time_0_X;

// data for depth of field
struct DOFvertexOutput 
{
 float4 HPosition : POSITION;
 float4 ViewSpacePos : TEXCOORD0;
};

// Output pixel values
struct pixelOutput {
 float4 col : COLOR;
};

// material properties
bool g_bDoubleSided : DoubleSided = false;

// used for ambient environment image-based lighting
float g_diffuseFactor = 0.1;
float g_specularFactor = 0.1;
bool g_bHasDiffuseEnvMap;
bool g_bHasSpecularEnvMap;
float g_diffuseEnvAngle = 0;
float g_specularEnvAngle = 0;
float4 g_envDiffuseColor = float4(1,1,1,1);
float4 g_envSpecularColor = float4(1,1,1,1);

texture2D diffuseEnvMap;
texture2D specularEnvMap;

// amount to extrude geometry for glow effect
float g_glowSize = 0;
bool g_bHasMask = false;
bool g_bConstGlow = false;

// maps a vector to normalized azimuth and elevation angles.
// angles are 0-1
// input vector is normalized
// +X is origin of X/Z plane for azimuthal angle
// +Y is up for elevation angle;
float2 CartesianToPolar( float3 vec )
{
 float3 nvec = normalize( vec );
 return float2( (atan2( nvec.z, nvec.x )+PI)*INVTWOPI, acos( nvec.y )*INVPI );
}

// debugging only: use this to help find nan/inf conditions
float CheckNan(float color)
{
 float outcolor = color;
 if (isinf(color))
  outcolor = 1;
 else if (isnan(color))
  outcolor = 0;
 return outcolor;
}

// debugging only: use this to help find nan/inf conditions
float3 CheckNan3(float3 color)
{
 float3 outcolor = color;
 if (isinf(color.r) || isinf(color.g) || isinf(color.b))
  outcolor = float3(1,0,0);
 else if (isnan(color.r) || isnan(color.g) || isnan(color.b))
  outcolor = float3(0,0,1);
 return outcolor;
}

// debugging only: use this to help find nan/inf conditions
float4 CheckNan4(float4 color)
{
 float4 outcolor = color;
 if (isinf(color.r) || isinf(color.g) || isinf(color.b) || isinf(color.a))
  outcolor = float4(1,0,0,1);
 else if (isnan(color.r) || isnan(color.g) || isnan(color.b) || isnan(color.a))
  outcolor = float4(0,0,1,1);
 return outcolor;
}

// compute the 3x3 tranform from world space to tangent space,
// transforming basis vectors to world space
// also send transpose of this matrix to the pixel shader
// so that it can transform the normal into world space to 
// compute the reflection vector for env mapping
void getTangentToWorldSpace(uniform float3x3 objToWorld,
 in float3 objTangent,
         in float3 objBinormal,
         in float3 objNormal,
         in float bumpMapScale,
//         out float3x3 worldToTangentSpace,
         out float3 tanToWorldX,
         out float3 tanToWorldY,
         out float3 tanToWorldZ)
{
 tanToWorldX = mul(objToWorld, objTangent).xyz;
 tanToWorldY = mul(objToWorld, objBinormal).xyz;
 tanToWorldZ = mul(objToWorld, objNormal).xyz;

/*
 float3 wTangent = mul(objToWorld, objTangent).xyz;
 float3 wBinormal = mul(objToWorld, objBinormal).xyz;
 float3 wNormal = mul(objToWorld, objNormal).xyz;
// worldToTangentSpace[0] = wTangent * bumpMapScale;
// worldToTangentSpace[1] = wBinormal * bumpMapScale;
// worldToTangentSpace[2] = wNormal;

 // assumption: inverse = transpose
 // why does bumpmapscale work like this? 
 // if the above uses bumpmapscale, shouldn't this one use 1/bumpmapscale?
 // yet somehow this works.
 tanToWorldX = float3(wTangent.x*bumpMapScale, wBinormal.x*bumpMapScale, wNormal.x);
 tanToWorldY = float3(wTangent.y*bumpMapScale, wBinormal.y*bumpMapScale, wNormal.y);
 tanToWorldZ = float3(wTangent.z*bumpMapScale, wBinormal.z*bumpMapScale, wNormal.z);
*/
}

TANGENT_MATRIX TransformTangents( uniform float3x3 mat,
         in float3 Tangent,
         in float3 Binormal,
         in float3 Normal )
{
 TANGENT_MATRIX tmat;
 tmat.X = mul( mat, Tangent);
 tmat.Y = mul( mat, Binormal);
 tmat.Z = mul( mat, Normal);
 return tmat;
}

float3 compress(float3 v)
{
 return 0.5 * v + 0.5.xxx;
}
float3 expand(float4 v)
{
 float4 v4 = 2.0*(v-0.5);
 return v4.xyz;
}
float3 expand(float3 v)
{
 return 2.0*(v-0.5);
}
float3 rotateAboutZ(float3 vec, float angle)
{
 if (angle == 0)
  return vec;
 else
 {
  float s,c;
  sincos(angle, s,c);
  return float3(vec.x*c - vec.y*s, 
   vec.x*s + vec.y*c,
   vec.z);
  //x' = x*cos q - y*sin q
  //y' = x*sin q + y*cos q
  //z' = z
 }
}
float3 rotateAboutY(float3 vec, float angle)
{
 if (angle == 0)
  return vec;
 else
 {
  float s,c;
  sincos(angle, s,c);
  return float3(vec.z*s + vec.x*c, 
   vec.y,
   vec.z*c - vec.x*s);
//  x' = z*sin q + x*cos q
//  y' = y
//  z' = z*cos q - x*sin q
 }
}
float3 rotateAboutX(float3 vec, float angle)
{
 if (angle == 0)
  return vec;
 else
 {
  float s,c;
  sincos(angle, s,c);
  return float3(vec.x,
   vec.y*c - vec.z*s, 
   vec.y*s + vec.z*c);
//  x' = x
//  y' = y*cos q - z*sin q
//  z' = y*sin q + z*cos q
 }
}

float4 Tex2DCombine(bool hasTex, sampler2D samp, float4 texCoord, float4 color)
{
// return (hasTex) ? color * tex2D(samp, texCoord) : color;
 float4 outCol = color;
 if (hasTex)
  outCol *= tex2D(samp, texCoord.xy);
 return outCol;
}
float4 Tex2DReplace(bool hasTex, sampler2D samp, float4 texCoord, float4 color)
{
// return (hasTex) ? tex2D(samp, texCoord) : color;
 float4 outCol = color;
 if (hasTex)
  outCol = tex2D(samp, texCoord.xy);
 return outCol;
}
float3 Tex2DNormal(bool hasTex, sampler2D samp, float4 texCoord)
{
// return (hasTex) ? normalize(expand(tex2D(samp, texCoord))) : float3(0,0,1);
 float3 bumpNormal = float3(0,0,1);
 if (hasTex)
  bumpNormal = normalize(expand(tex2D(samp, texCoord.xy)));
 return bumpNormal;
}

float4 SampleEnvironment(float3 worldEyeDir, float3 worldNormal,
       uniform sampler2D CubeMap)
{
 // world eye dir points FROM shade point TO eye pos

 // do world space reflection
 float nDotV = dot(worldEyeDir, worldNormal);
 //R = 2*N*(L.N)-L
 float3 reflVect = 2.0 * nDotV * worldNormal - worldEyeDir;
 return tex2Dlod(CubeMap, float4(CartesianToPolar(reflVect),0,0));
}

float4 SampleEnvironment(float3 worldEyeDir, float3 worldNormal,
       uniform sampler2D CubeMap, float angle)
{
 // do world space reflection
 float nDotV = dot(worldEyeDir, worldNormal);
 //R = 2*N*(L.N)-L
 float3 reflVect = 2.0 * nDotV * worldNormal - worldEyeDir;
 // Rotate about the Y axis using reflective vector
 float3 rotateVec = rotateAboutY(reflVect, angle);

 return tex2Dlod(CubeMap, float4(CartesianToPolar(rotateVec),0,0));
}

float4 SampleEnvironmentLOD(float3 worldEyeDir, float3 worldNormal,
       uniform sampler2D CubeMap, float angle, float mipLOD)
{
 // do world space reflection
 float nDotV = dot(worldEyeDir, worldNormal);
 //R = 2*N*(L.N)-L
 float3 reflVect = 2.0 * nDotV * worldNormal - worldEyeDir;
 // Rotate about the Y axis using reflective vector
 float3 rotateVec = rotateAboutY(reflVect, angle);

 return tex2Dlod(CubeMap, float4(CartesianToPolar(rotateVec), 0, mipLOD));
}

float4 SampleEnvDiffuse(float3 worldNormal,
       uniform sampler2D CubeMap)
{
 return tex2Dlod(CubeMap, float4(CartesianToPolar(worldNormal),0,0));
}
float4 SampleEnvDiffuse(float3 worldNormal,
       uniform sampler2D CubeMap, float angle)
{
 worldNormal = rotateAboutY(worldNormal, angle);
 return tex2Dlod(CubeMap, float4(CartesianToPolar(worldNormal),0,0));
}
float4 SampleEnvDiffuseLOD(float3 worldNormal,
       uniform sampler2D CubeMap, float angle, float mipLod)
{
 worldNormal = rotateAboutY(worldNormal, angle);
 return tex2Dlod(CubeMap, float4(CartesianToPolar(worldNormal), 0, mipLod));
}

// Return a color that approximates the diffuse contribution
// of the environment map. Used in non-shadow passes.
float4 envmap_approximation(float envMapDffuseFactor)
{
 return float4(0.8 * g_envDiffuseColor.rgb * envMapDffuseFactor, 0);
// float scaled_factor = 0.8 * envMapDffuseFactor;
// return float4(scaled_factor, scaled_factor, scaled_factor, 0);
}

float3 PhongDiffuse(float3 normal, float3 lightDir, float3 lDiffColor, float3 sDiffColor)
{
 float cosine = saturate(dot(normal,lightDir));
 return cosine * (lDiffColor * sDiffColor);
}

float3 PhongSpecular(float3 normal, float3 lightDir, float3 eyeDir, 
 float3 lSpecColor, float3 sSpecColor, float sSpecPower)
{
 // eyeDir is passed as dir FROM shade point TO eye
 // lightDir is passed as dir FROM shade point TO light

 float specComp = saturate(dot(normal,normalize(lightDir + eyeDir)));
 // keep SpecPower from being 0 for numerical stability
 // alternately, can use 
  //if (specComp > 0)
  // specComp = (pow(specComp, sSpecPower));
 // instead of:
 specComp = (dot(normal, lightDir)>0) * (pow(specComp, max(0.001, sSpecPower)));
 return specComp * (lSpecColor * sSpecColor);
}

// fresnel approximation
float fastFresnel(float NdotE, float R0, float power)
{
// R(theta) = (1/2) ((g-c)/(g+c))^2 (1 + [ (c(g+c)-(eta)^2)/(c(g-c)+ (eta)^2) ]2)
 // approximate:
 // R(theta) ~= Ra(theta) = R(0) + (1-R(0))*(1-cos(theta))^5
 return R0 + (1.0-R0)*pow(abs(1.0 - saturate(NdotE)), power);
} 

float fresnel(float NdotE, float eta)
{
 // eta = ni/nt.
 // ni is usually 1 for air, and nt would be the IOR of the material.
 // that means we might want 1/IOR for eta.

 // R(theta) = (1/2) ((g-c)/(g+c))^2 (1 + [ (c(g+c)-(eta)^2)/(c(g-c)+ (eta)^2) ]2)
 // where:
 // cos(theta) = NdotV
 // c = eta*cos(theta)
 // g = sqrt(1 + c^2 - eta^2)

 // Note: compute R0 on the CPU and provide as a
 // constant; it is more efficient than computing R0 in
 // the shader. R0 is:
 // when theta = 0, c = eta and g = 1
 float R0 = (1.0-eta)*(1.0-eta)/((1.0+eta)*(1.0+eta));
 //float R0 = pow(1.0-eta, 2.0) / pow(1.0+eta, 2.0);
 
 // eye and normal are assumed to be normalized
 return fastFresnel(NdotE, R0, 5.0);
}


// g_vDofParams
// x = near blur depth
// y = near focal plane depth
// z = far focal plane depth
// w = far blur depth
float4 g_vDofParams = float4(30, 60, 80, 120);

// blurriness cutoff constant for objects behind focal plane
//  (1.0 = max blur, 0.0 = max sharpness)
float g_fDofBlurCutoff = 1;
float ComputeDepthBlur(float depth /* in view space */)
{
 float f=0;

 if (depth < g_vDofParams.y)
 {
  // scale depth value between near blur distance and focal distance to [-1,0]
  f = (depth-g_vDofParams.y)/(g_vDofParams.y-g_vDofParams.x);
  f = clamp (f, -1, 0);
 }
 else if (depth > g_vDofParams.z)
 {
  // scale depth value between focal distance and far blur distance to [0,1]
  f = (depth-g_vDofParams.z)/(g_vDofParams.w-g_vDofParams.z);
  // clamp the far blur to a maximum blurriness
  f = clamp (f, 0, g_fDofBlurCutoff);
 }

 // scale and bias into [0,1] range
 return ((f * 0.5f) + 0.5f);
}
pixelOutput DOFPrep_PS(DOFvertexOutput IN) 
{
 pixelOutput OUT; 
 float bl = ComputeDepthBlur(IN.ViewSpacePos.z); 
// float bl = (IN.ViewSpacePos.z); 
 OUT.col = float4(bl,bl,bl,bl);
 return OUT;
}

pixelOutput simpleMattePS()
{
 pixelOutput OUT; 
 OUT.col = float4(1,1,1,1);
 return OUT;
}

// convert vertex uv coordinate to clip space position for texture baking.
float4 BakeVertex(in float4 i_UV, in float4x4 wvp)
{
 // this step assures that the baked texture captures 
 // the entire texture space of a mesh that has pre-tiled uvs.
 float2 untiledUV = i_UV.xy * g_bakeTransform.xy + g_bakeTransform.zw;

 float4 o_hPos;

// o_hPos.xy = untiledUV*2 - float2(1,1);
// o_hPos.y = -o_hPos.y;
// o_hPos.zw = i_UV.zw;

 untiledUV -= 0.5;
 o_hPos = mul(wvp, float4(untiledUV,0.5,1));


 return o_hPos;
}

// decide how to best get the vertex to clip space, and then do it!
float4 TransformVertex(in float4 i_Po, in float4 i_UV, in float4x4 wvp)
{
 float4 o_hPos;
 if (g_bake)
  o_hPos = BakeVertex(i_UV, wvp);
 else
  o_hPos = mul(wvp, i_Po);
 return o_hPos;
}


/* data from application vertex buffer */
struct vertexData {
 float3 Position : POSITION;
 float3 Normal : NORMAL;
 float4 UV  : TEXCOORD0;
 float3 T  : TANGENT;
 float3 B  : BINORMAL;
};
struct shadingData {
 float4 HPosition : POSITION;
 float4 TexCoord0 : TEXCOORD0;
 float4 UV   : TEXCOORD1;
 float3 WorldPos  : TEXCOORD2;
 float3 WorldTanMatrixX : TEXCOORD3;
 float3 WorldTanMatrixY : TEXCOORD4;
 float3 WorldTanMatrixZ : TEXCOORD5;
};
shadingData standardVS(vertexData IN,
 uniform float4x4 WorldViewProj,
 uniform float4x4 WorldIT,
 uniform float4x4 World,
 uniform float4x4 ViewIT,
 uniform float BumpMapScale,
 uniform float GlowSize
) {
 shadingData OUT;
 
 // output position in proj space
 float4 Po = float4(IN.Position.xyz + IN.Normal*GlowSize, 1.0);
 OUT.HPosition = TransformVertex(Po, IN.UV, WorldViewProj);
 
 // transform position to world space and get vector to light
 float3 Pw = mul(World, Po).xyz;
 OUT.WorldPos = Pw;

 // theoretically this should be using WorldIT! Who dares to change it?
 getTangentToWorldSpace((float3x3)World,IN.T,IN.B,IN.Normal,BumpMapScale, 
  OUT.WorldTanMatrixX,OUT.WorldTanMatrixY,OUT.WorldTanMatrixZ);
 
 // decal and bump texture coords
 OUT.TexCoord0 = mul(g_uvTransform, IN.UV);
 OUT.UV = IN.UV;
 
 return OUT;
}

// these filter funcs work for floats, but maybe not for vector quantities.
#define MINFILTERWIDTH 0.000001
float filterwidth(float x) { return max(abs(ddx(x)) + abs(ddy(x)), MINFILTERWIDTH); }
float filterarea(float x) { return (abs(ddx(x)) * abs(ddy(x))); }
float filteredstep(float edge, float x, float w) { return clamp(((x)+(w)/2-(edge))/(w), 0, 1); }


/***************************** eof ***/
/*****************************************************************************
** Tessellate.h
**
** ATI Hardware Tessellation functions
**
** Extra Large Technology
** Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifndef _GLOBALS_
#include "Globals.h"
#endif

//#define MESHDATATEXTURESIZE 256.0f     //make sure this is a multiple of 16
//#define TEXELSIZE (1.0f/MESHDATATEXTURESIZE)
//#define HALFTEXELSIZE (TEXELSIZE/2.0f)
//const float4 Stride = float4( TEXELSIZE, 0, 0, 0);
#define PATCHSIZE 48  //number of float4 elements for a patch (aka vertex texture stride)
#define UPATCHOFFSET 16  //Offset from the patch index to the first U Tangent patch
#define VPATCHOFFSET 32  //Offset from the patch index to the first V Tangent patch

//#define USETANGENTPATCHES

//this applies the transformation after the tessellation to displace the final vertex
#define APPLY_DISPLACEMENT

//this alters the tangent space based on the derived vectors from the displacement map (Sobel Filter)
#define CALCULATE_BUMP_NORMAL

#define BUMP_NORMAL_FALLBACK

//stores 16 float4 control points in sequence to define a bi-cubic Bezier patch mesh
texture2D meshDataTexture : meshdatamap;
float  g_MeshDataTextureWidth = 256.0f; //make sure this is a multiple of 16
float  g_MeshDataTextureHeight = 64.0f;

sampler meshDataSampler = 
sampler_state
{
 Texture = <meshDataTexture>;
 AddressU = CLAMP;
 AddressV = CLAMP;
 MipFilter = NONE;
 MinFilter = POINT;
 MagFilter = POINT;
};

//----for displacement mapping---------

bool g_hasDisplacementMap = false;
float g_DisplacementScale;
float g_DisplacementBias;
float g_DisplacementBlur;
float2 g_DisplacementMapSize;
float2 g_ObjectUVScale;
texture2D g_DisplacementMap;

/*
float g_DisplacementScale : DisplacementScale
<
 string SasUiControl = "Slider";
 string SasUiLabel = "Scale";
 string SasUiDescription = "Sets the magnitude of the displacement.";
 string UiCategory = "Displacement";
 float SasUiMin = 0.0;
 float SasUiMax = 1.0;
> = 0.0f;

float g_DisplacementBias : DisplacementBias
<
 string SasUiControl = "Slider";
 string SasUiLabel = "Bias";
 string SasUiDescription = "Sets the offset of the displacement.";
 string UiCategory = "Displacement";
 float SasUiMin = -1.0;
 float SasUiMax = 1.0;
> = 0.0f;

float g_DisplacementBlur : DisplacementBlur
<
 string SasUiControl = "Slider";
 string SasUiLabel = "Soften";
 string SasUiDescription = "Softens the displacement texture.";
 string UiCategory = "Displacement";
 float SasUiMin = 0.0;
 float SasUiMax = 1.0;
> = 0.0f;

texture2D g_DisplacementMap : DisplacementMap
<
 string SasUiControl = "FilePicker";
 string SasUiLabel = "Displacement Map";
 string SasUiDescription = "Uses only the (R) color channel to define height.";
 string UiCategory = "Displacement";
 string ExistVar = "hasDisplacementMap";
>;
*/
sampler2D displacementSampler = sampler_state
{
 Texture = <g_DisplacementMap>;
 MipFilter = LINEAR;
 MinFilter = Anisotropic;
 MaxAnisotropy = 16;
 MagFilter = LINEAR;
 AddressU = WRAP;
 AddressV = WRAP;
};

//#define USE_QUADS

/*********** structures ******/

#ifdef USE_QUADS
struct VS_INPUT_TESS //quads
{
 // Cartesian coordinates
 float4 vCartesianCoordinates : BLENDWEIGHT0;
 // Indices
 uint4 vIndices  : BLENDINDICES0;

 // Superprim Vertex 0
// float3 Position0 : POSITION0;
 float2 UV0   : TEXCOORD0;

 // Superprim Vertex 1
// float3 Position1 : POSITION4;
 float2 UV1   : TEXCOORD4;

 // Superprim Vertex 2
// float3 Position2 : POSITION8;
 float2 UV2   : TEXCOORD8;

 // Superprim Vertex 3
// float3 Position3 : POSITION12;
 float2 UV3   : TEXCOORD12;
};

struct VS_INPUT_TESS2 //quads
{
 // Cartesian coordinates
 float4 vCartesianCoordinates : BLENDWEIGHT0;

 // Superprim Vertex 0
 float4 Position0 : POSITION0;
 float3 Normal0 : NORMAL0;
 float4 UV0   : TEXCOORD0;

 // Superprim Vertex 1
 float4 Position1 : POSITION4;
 float3 Normal1 : NORMAL4;
 float4 UV1   : TEXCOORD4;

 // Superprim Vertex 2
 float4 Position2 : POSITION8;
 float3 Normal2 : NORMAL8;
 float4 UV2   : TEXCOORD8;

 // Superprim Vertex 3
 float4 Position3 : POSITION12;
 float3 Normal3 : NORMAL12;
 float4 UV3   : TEXCOORD12;
};

#else//USE_QUADS
struct VS_INPUT_TESS  //triangles
{
 float3 vBarycentric : BLENDWEIGHT0;

 // Superprim Vertex 0
 float4 Position0 : POSITION0;
 float3 Normal0 : NORMAL0;
 float4 UV0   : TEXCOORD0;
 float3 Tangent0 : TANGENT0;
 float3 Binormal0 : BINORMAL0;

 // Superprim Vertex 1
 float4 Position1 : POSITION4;
 float3 Normal1 : NORMAL4;
 float4 UV1   : TEXCOORD4;
 float3 Tangent1 : TANGENT4;
 float3 Binormal1 : BINORMAL4;

 // Superprim Vertex 2
 float4 Position2 : POSITION8;
 float3 Normal2 : NORMAL8;
 float4 UV2   : TEXCOORD8;
 float3 Tangent2 : TANGENT8;
 float3 Binormal2 : BINORMAL8;
};
#endif//USE_QUADS

//----Will sample the displacement map and translate along the normal.
//----With the exception of floating point textures all samples are scaled to real world
//------value and biased adjust it's center point. A blur into mip level is also provided.
float3 DisplaceVertex( float3 Pos, float3 Normal, float2 TexCoords )
{
 // Read height from R component
 float height = tex2Dlod( displacementSampler, 
  float4( TexCoords, 0.0f, g_DisplacementBlur*10.0f)).x; //blur handles 10 mip levels

 // Vertex in clip space
 return Pos + (Normal * ((height - g_DisplacementBias) * g_DisplacementScale));
}

/*********** data ******/
bool hasHardwareTessellation = true; //if this flag is defined in a shader then hardware tessellation shaders are supported

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
// string SasUiControl = "CheckBox";
// string SasUiLabel = "Flat Tessellate";
// string SasUiDescription = "To disable PN Smoothing.";
// string UiCategory = "Tessellation";
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
// (0,1) (1,1)
// V3---V2
// | |
// | |
// V0---V1
// (0,0) (1,0)
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
 vParametricCoordinates.xy = UV1 * vInputWeights.y + UV2 * vInputWeights.w;

 // Convenience variables: z and w contain (1-u) and (1-v) respectively
 vParametricCoordinates.zw = 1.0 - vParametricCoordinates.xy;

 return vParametricCoordinates;
}


//---------------Linear Interpolation----------------
#ifdef USE_QUADS
/*
float3 GetLinearQuadVertex( VS_INPUT_TESS In )
{
 // Prepare interpolation factors: u*v, (1-u)*v, (1-u)*(1-v), u*(1-v)
 float4 fInterpolationFactors = In.vCartesianCoordinates.xzzx * In.vCartesianCoordinates.yyww;

 //quadlinear interpolate
 return In.Position0 * fInterpolationFactors.x + In.Position1 * fInterpolationFactors.y + In.Position2 * fInterpolationFactors.z + In.Position3 * fInterpolationFactors.w;
}

float3 GetLinearQuadVertexLU( VS_INPUT_TESS In )
{
 // Prepare interpolation factors: u*v, (1-u)*v, (1-u)*(1-v), u*(1-v)
 float4 fInterpolationFactors = In.vCartesianCoordinates.xzzx * In.vCartesianCoordinates.yyww;

 //lookup control points
 float uPatchNumber = In.vIndices.x / 4.0f;

 float3 P1 = tex2Dlod(meshDataSampler, float4( uPatchNumber*16.0f/255.0f, 0, 0, 0)).xyz;
 float3 P2 = tex2Dlod(meshDataSampler, float4( (uPatchNumber*16.0f+1)/255.0f, 0, 0, 0)).xyz;
 float3 P3 = tex2Dlod(meshDataSampler, float4( (uPatchNumber*16.0f+2)/255.0f, 0, 0, 0)).xyz;
 float3 P4 = tex2Dlod(meshDataSampler, float4( (uPatchNumber*16.0f+3)/255.0f, 0, 0, 0)).xyz;

// float3 P1 = SampleVertexData( uPatchNumber );
// float3 P2 = SampleVertexData( uPatchNumber+1.0f );
// float3 P3 = SampleVertexData( uPatchNumber+2.0f );
// float3 P4 = SampleVertexData( uPatchNumber+3.0f );
// float3 P1 = SampleVertexData( 0.0f );
// float3 P2 = SampleVertexData( 1.0f );
// float3 P3 = SampleVertexData( 2.0f );
// float3 P4 = SampleVertexData( 3.0f );
// float3 P1 = SampleVertexData( In.vIndices.x );
// float3 P2 = SampleVertexData( In.vIndices.x+1.0f );
// float3 P3 = SampleVertexData( In.vIndices.x+2.0f );
// float3 P4 = SampleVertexData( In.vIndices.x+3.0f );
//  float3 P1 = SampleVertexData( In.vIndices.x*4.0f ).xyz;
 // float3 P2 = SampleVertexData( In.vIndices.x*16+3.0f ).xyz;
 // float3 P3 = SampleVertexData( In.vIndices.x*16+12.0f ).xyz;
 // float3 P4 = SampleVertexData( In.vIndices.x*16+15.0f ).xyz;

 // Fetch extra vertex data from the three original vertices 
// float4 P1 = tex2Dlod(meshDataSampler, float4( (modulo(In.vIndices.x, 256))/255.0, (In.vIndices.x/256.0)/256.0, 0, 0)).xyz;
// float4 P2 = tex2Dlod(meshDataSampler, float4( (modulo(In.vIndices.y, 256))/255.0, (In.vIndices.y/256.0)/256.0, 0, 0)).xyz;
// float4 P3 = tex2Dlod(meshDataSampler, float4( (modulo(In.vIndices.z, 256))/255.0, (In.vIndices.z/256.0)/256.0, 0, 0)).xyz;
// float4 P4 = tex2Dlod(meshDataSampler, float4( (modulo(In.vIndices.w, 256))/255.0, (In.vIndices.w/256.0)/256.0, 0, 0)).xyz;

  return P1 * fInterpolationFactors.x + P2 * fInterpolationFactors.y + P3 * fInterpolationFactors.z + P4 * fInterpolationFactors.w;
// return (P1 + In.Position0) * fInterpolationFactors.x + (P2+In.Position1) * fInterpolationFactors.y + (P3+In.Position2) * fInterpolationFactors.z + (P4+In.Position3) * fInterpolationFactors.w;

}


float3 GetLinearQuadNormal( VS_INPUT_TESS In )
{
 // Prepare interpolation factors: u*v, (1-u)*v, (1-u)*(1-v), u*(1-v)
 float4 fInterpolationFactors = In.vCartesianCoordinates.xzzx * In.vCartesianCoordinates.yyww;

 //quadlinear interpolate
 return normalize(In.Normal0 * fInterpolationFactors.x + In.Normal1 * fInterpolationFactors.y + In.Normal2 * fInterpolationFactors.z + In.Normal3 * fInterpolationFactors.w);
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

*/
float2 GetLinearQuadTexture( VS_INPUT_TESS In )
{
 // Prepare interpolation factors: u*v, (1-u)*v, (1-u)*(1-v), u*(1-v)
 float4 fInterpolationFactors = In.vCartesianCoordinates.xzzx * In.vCartesianCoordinates.yyww;

 //quadlinear interpolate
 return In.UV2 * fInterpolationFactors.x + In.UV3 * fInterpolationFactors.y + In.UV0 * fInterpolationFactors.z + In.UV1 * fInterpolationFactors.w;
// return In.UV0 * In.vCartesianCoordinates.x + In.UV1 * In.vCartesianCoordinates.y + In.UV2 * In.vCartesianCoordinates.z + In.UV3 * In.vCartesianCoordinates.w;
}

#else//USE_QUADS
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

float4 GetLinearTriangleTexture( VS_INPUT_TESS In )
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

#endif//USE_QUADS
/*
//---------------Cubic Interpolation----------------
float3 GetPNQuadVertexLU2( VS_INPUT_TESS In )
{
 float3 Vert = float3(0,0,0);
 float3  fControlPoints[16];
 float4  vParametricCoordinates; // U, V, 1-U, 1-V 

 //lookup control points
 float uPatchNumber = floor( In.vIndices.x / 4 ) * 16;

 fControlPoints[0] = In.Position0 + SampleVertexData( uPatchNumber ); //1
 fControlPoints[1] = SampleVertexData( uPatchNumber+1 ); //2
 fControlPoints[2] = SampleVertexData( uPatchNumber+2 ); //3
 fControlPoints[3] = SampleVertexData( uPatchNumber+3 ); //4
 fControlPoints[4] = SampleVertexData( uPatchNumber+4 ); //5
 fControlPoints[5] = SampleVertexData( uPatchNumber+5 ); //6
 fControlPoints[6] = SampleVertexData( uPatchNumber+6 ); //7
 fControlPoints[7] = SampleVertexData( uPatchNumber+7 ); //8
 fControlPoints[8] = SampleVertexData( uPatchNumber+8 ); //9
 fControlPoints[9] = SampleVertexData( uPatchNumber+9 ); //10
 fControlPoints[10] = SampleVertexData( uPatchNumber+10 ); //11
 fControlPoints[11] = SampleVertexData( uPatchNumber+11 ); //12
 fControlPoints[12] = SampleVertexData( uPatchNumber+12 );  //13
 fControlPoints[13] = SampleVertexData( uPatchNumber+13 ); //14
 fControlPoints[14] = SampleVertexData( uPatchNumber+14 ); //15
 fControlPoints[15] = SampleVertexData( uPatchNumber+15 ); //16

 // Calculate parametric coordinates for patch evalution 
 vParametricCoordinates = CalculatePatchParametricCoordinates( In.vCartesianCoordinates, In.vIndices );

 // Bezier patch is defined by:
 //
 // ( 1 0 0 0 ) ( P00 P01 P02 P03 ) ( 1 -3 3 -1 ) ( 1 )
 // P(u, v) = [1, u, u*u, u*u*u] * ( -3 3 0 0 ) * ( P10 P11 P12 P13 ) * ( 0 3 -6 3 ) * ( v )
 // ( 3 -6 3 0 ) ( P20 P21 P22 P23 )  ( 0 0 3 -3 ) ( v*v )
 // ( -1 3 -3 1 ) ( P30 P31 P32 P33 )  ( 0 0 0 -1 ) ( v*v*v )

 // Calculate:
 // ( 1 0 0 0 )    ( 1 -3 3 -1 ) ( 1 )
 // [1, u, u*u, u*u*u] * ( -3 3 0 0 )  and  ( 0 3 -6 3 ) * ( v )
 // ( 3 -6 3 0 )    ( 0 0 3 -3 ) ( v*v )
 // ( -1 3 -3 1 )    ( 0 0 0 -1 ) ( v*v*v )

 // The below straight multiplication method for the computation of polynomials
 // is the one yielding the less instructions (better for performance)
 float4 fPolynomialU = float4(vParametricCoordinates.z*vParametricCoordinates.z*vParametricCoordinates.z,    // (1-u)^3
        3.0 * vParametricCoordinates.x * vParametricCoordinates.z * vParametricCoordinates.z,  // 3*u*(1-u)^2
        3.0 * vParametricCoordinates.x * vParametricCoordinates.x * vParametricCoordinates.z,  // 3*u^2*(1-u)
        vParametricCoordinates.x * vParametricCoordinates.x * vParametricCoordinates.x);   // u^3
 float4 fPolynomialV = float4(vParametricCoordinates.w*vParametricCoordinates.w*vParametricCoordinates.w,    // (1-v)^3
        3.0 * vParametricCoordinates.y * vParametricCoordinates.w * vParametricCoordinates.w,  // 3*v*(1-v)^2
        3.0 * vParametricCoordinates.y * vParametricCoordinates.y * vParametricCoordinates.w,  // 3*v^2*(1-v)
        vParametricCoordinates.y * vParametricCoordinates.y * vParametricCoordinates.y);   // v^3

 // Evaluate patch
 Vert = fPolynomialU.x * ( fControlPoints[0].xyz * fPolynomialV.x + fControlPoints[4].xyz * fPolynomialV.y + fControlPoints[8].xyz * fPolynomialV.z + fControlPoints[12].xyz * fPolynomialV.w );
 Vert += fPolynomialU.y * ( fControlPoints[1].xyz * fPolynomialV.x + fControlPoints[5].xyz * fPolynomialV.y + fControlPoints[9].xyz * fPolynomialV.z + fControlPoints[13].xyz * fPolynomialV.w );
 Vert += fPolynomialU.z * ( fControlPoints[2].xyz * fPolynomialV.x + fControlPoints[6].xyz * fPolynomialV.y + fControlPoints[10].xyz * fPolynomialV.z + fControlPoints[14].xyz * fPolynomialV.w );
 Vert += fPolynomialU.w * ( fControlPoints[3].xyz * fPolynomialV.x + fControlPoints[7].xyz * fPolynomialV.y + fControlPoints[11].xyz * fPolynomialV.z + fControlPoints[15].xyz * fPolynomialV.w );

 return Vert;
}
*/
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
 V += VBasis.x * (CP[0] * UBasis.x + CP[1] * UBasis.y + CP[2] * UBasis.z + CP[3] * UBasis.w);
 V += VBasis.y * (CP[4] * UBasis.x + CP[5] * UBasis.y + CP[6] * UBasis.z + CP[7] * UBasis.w);
 V += VBasis.z * (CP[8] * UBasis.x + CP[9] * UBasis.y + CP[10] * UBasis.z + CP[11] * UBasis.w);
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
// float3 vQuadB[3];
// float3 vCubic[4];
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
/*
 // two internal edges - this is where work happens...
 float3 vA,vB,vC,vD,vE;
 float fC0,fC1;
 vQuad[1] = 3.0f*(vIn[2*cX+2*cY]-vIn[1*cX+2*cY]);
 // also do "second" scan line
 vQuadB[1] = 3.0f*(vIn[2*cX+1*cY]-vIn[1*cX+1*cY]);

 vD = 3.0f*(vIn[1*cX + 2*cY] - vIn[0*cX + 2*cY]);
 vE = 3.0f*(vIn[1*cX + 1*cY] - vIn[0*cX + 1*cY]); // used later...

 fC0 = fCWts.w;
 fC1 = fCWts.x;

 // sign flip
 vA = -vCorner[3];
 vB = 3.0f*(vIn[0*cX + 1*cY] - vIn[0*cX + 2*cY]);
 vC = -vCorner[0];

 vQuad[0] = 1.0f/3.0f*(2.0f*fC0*vB - fC1*vA) + vD;
 vQuadB[0] = 1.0f/3.0f*(fC0*vC - 2.0f*fC1*vB) + vE;

 // do end of strip - same as before, but stuff is switched around...
 vC = vCorner[2];
 vB = 3.0f*(vIn[3*cX + 2*cY] - vIn[3*cX + 1*cY]);
 vA = vCorner[1];

 vD = 3.0f*(vIn[2*cX + 1*cY] - vIn[3*cX + 1*cY]);
 vE = 3.0f*(vIn[2*cX + 2*cY] - vIn[3*cX + 2*cY]);

 fC0 = fCWts.y;
 fC1 = fCWts.z;

 vQuadB[2] = 1.0f/3.0f*(2.0f*fC0*vB - fC1*vA) + vD;
 vQuad[2] = 1.0f/3.0f*(fC0*vC - 2.0f*fC1*vB) + vE;

 vQuadB[2] *= -1.0f;
 vQuad[2] *= -1.0f;

 BezierRaise(vQuad,vCubic);

 vOut[0*cX + 2*cY] = vCubic[0];
 vOut[1*cX + 2*cY] = vCubic[1];
 vOut[2*cX + 2*cY] = vCubic[2];
 vOut[3*cX + 2*cY] = vCubic[3];

 BezierRaise(vQuadB,vCubic);

 vOut[0*cX + 1*cY] = vCubic[0];
 vOut[1*cX + 1*cY] = vCubic[1];
 vOut[2*cX + 1*cY] = vCubic[2];
 vOut[3*cX + 1*cY] = vCubic[3];
*/
}
/*
void LoadTangentPoints( in float4 UVCoord, in float3 CP[16], out float3 UP[16], out float3 VP[16] )
{
 VP[1] = float3(0,0,0);
 VP[2] = float3(0,0,0);
 VP[4] = float3(0,0,0);
 VP[5] = float3(0,0,0);
 VP[6] = float3(0,0,0);
 VP[7] = float3(0,0,0);
 VP[8] = float3(0,0,0);
 VP[9] = float3(0,0,0);
 VP[10] = float3(0,0,0);
 VP[11] = float3(0,0,0);
 VP[13] = float3(0,0,0);
 VP[14] = float3(0,0,0);
 UP[1] = float3(0,0,0);
 UP[2] = float3(0,0,0);
 UP[4] = float3(0,0,0);
 UP[5] = float3(0,0,0);
 UP[6] = float3(0,0,0);
 UP[7] = float3(0,0,0);
 UP[8] = float3(0,0,0);
 UP[9] = float3(0,0,0);
 UP[10] = float3(0,0,0);
 UP[11] = float3(0,0,0);
 UP[13] = float3(0,0,0);
 UP[14] = float3(0,0,0);

 VP[0] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;
 VP[3] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;
 VP[15] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;
 VP[12] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;

// UP[12] = float3(0,0,0);
 UP[15] = float3(0,0,0);

 UP[0] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;
 UP[3] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;
 UP[15] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;
 UP[12] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;

 float4 weights = tex2Dlod( meshDataSampler, UVCoord );
 float fCWts[4];
 fCWts[0] = weights.x;
 fCWts[1] = weights.y;
 fCWts[2] = weights.z;
 fCWts[3] = weights.w;

 float3 vCorner[4];
 float3 vCornerLocal[4];

 vCorner[0] = VP[0];
 vCorner[1] = VP[3];
 vCorner[2] = VP[15];
 vCorner[3] = VP[12];
 vCornerLocal[0] = UP[0];
 vCornerLocal[1] = UP[3];
 vCornerLocal[2] = UP[12];
 vCornerLocal[3] = UP[15];

 ComputeTanPatch(CP,UP,fCWts,vCorner,vCornerLocal,1,4);

 fCWts[3] = weights.y;
 fCWts[1] = weights.w;

 vCorner[0] = UP[0];
 vCorner[3] = UP[3];
 vCorner[2] = UP[15];
 vCorner[1] = UP[12];
 vCornerLocal[0] = VP[0];
 vCornerLocal[1] = VP[12];
 vCornerLocal[2] = VP[3];
 vCornerLocal[3] = VP[15];

 ComputeTanPatch(CP,VP,fCWts,vCorner,vCornerLocal,4,1);

}

void LoadUTangentPoints( in float4 UVCoord, in float3 CP[16], out float3 UP[16] )
{
// UP[1] = float3(0,0,0);
// UP[2] = float3(0,0,0);
 UP[4] = float3(0,0,0);
 UP[5] = float3(0,0,0);
 UP[6] = float3(0,0,0);
 UP[7] = float3(0,0,0);
 UP[8] = float3(0,0,0);
 UP[9] = float3(0,0,0);
 UP[10] = float3(0,0,0);
 UP[11] = float3(0,0,0);
 UP[13] = float3(0,0,0);
 UP[14] = float3(0,0,0);

 float3 vCorner[4];
 vCorner[0] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;
 vCorner[1] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;
 vCorner[2] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;
 vCorner[3] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;

 UP[0] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;
 UP[3] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;
 UP[15] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;
 UP[12] = tex2Dlod( meshDataSampler, UVCoord );
 UVCoord += Stride;

 
 float4 weights = tex2Dlod( meshDataSampler, UVCoord );

 float3 vCornerLocal[4];

 vCornerLocal[0] = UP[0];
 vCornerLocal[1] = UP[3];
 vCornerLocal[2] = UP[12];
 vCornerLocal[3] = UP[15];

 ComputeTanPatch(CP,UP,weights,vCorner,vCornerLocal,1,4);

 fCWts[3] = weights.y;
 fCWts[1] = weights.w;

 vCorner[0] = UP[0];
 vCorner[3] = UP[3];
 vCorner[2] = UP[15];
 vCorner[1] = UP[12];
 vCornerLocal[0] = VP[0];
 vCornerLocal[1] = VP[12];
 vCornerLocal[2] = VP[3];
 vCornerLocal[3] = VP[15];

 ComputeTanPatch(CP,VP,weights,vCorner,vCornerLocal,4,1);
}
*/

#ifdef USE_QUADS
void LoadPoints( inout float3 CP[16], in float uPatchNumber, inout float4 UV, in VS_INPUT_TESS In )
//void LoadPoints( inout float3 CP[16], in float uPatchNumber )
{
 const float InvWidth = 1.0f / g_MeshDataTextureWidth;
 const float InvWidthS = 1.0f / (g_MeshDataTextureWidth-1);
 const float InvHeight = 1.0f / g_MeshDataTextureHeight;
 const float4 MeshTextureStride = float4( InvWidth, 0, 0, 0 );

 int ID = (In.vIndices.x / 4) * 48;
 float4 CPCoord = float4(modulo( ID, g_MeshDataTextureWidth ) * InvWidthS, ID*InvWidth*InvHeight, 0, 0 );
// float4 CPCoord = float4(modulo( ID, 608)/607.0f, (ID/608.0f)/605.0f, 0, 0);
// float4 CPCoord = float4(frac(ID*InvWidthS), ID*InvWidth*InvHeight, 0, 0);
// float4 CPCoord = float4( frac(uPatchNumber*InvWidthS), floor(uPatchNumber*InvWidth)*InvHeight, 0, 0 );
/* if( CPCoord.x > 0.975329f )
 {
  UV += float4( 1, 0, 0, 0 );
 }
 if( CPCoord.y > 0.998347f )
 {
  UV += float4( 0, 1, 0, 0 );
 }
*/
 [unroll]for( int i = 0; i < 16; i++, CPCoord += MeshTextureStride )
 {
  CP[ i ] = tex2Dlod( meshDataSampler, CPCoord );
/*  if( length( CP[i] ) < 1.0f )
  {
   UV += float4( 1, 1, 0, 0 );
  }*/
 }
}

float3 GetQuadPatchLUVertex( VS_INPUT_TESS In, out float3 Tan, out float3 Bitan, inout float4 UV ) //testing
//float3 GetQuadPatchLUVertex( VS_INPUT_TESS In, out float3 Tan, out float3 Bitan )
{
 float3 CP[16];  //16 control points, reuse for U and V to save on temp registers
 float3 Vert;

 //convert Cartesian interpolants into U and V parametric terms from 0 to 1
 float4 parms = CalculatePatchParametricCoordinates( In.vCartesianCoordinates, In.vIndices );

 //calculate the Bernstein polynomial vectors
 float4 U = BernsteinPoly( parms.x, parms.z );// Define the basis using parameter u
 float4 V = BernsteinPoly( parms.y, parms.w );// Define the basis using parameter v

 //Use x index to calculate the patch index
 float uPatchNumber = floor( In.vIndices.x / 4.0f ) * 48.0f;

 LoadPoints( CP, uPatchNumber, UV, In );    //load the vertex control points from the texture
 Vert = CalcCubicBezierPatch( CP, U, V ); //calculate the bezier vertex from the control points and the weight vectors

#ifdef USETANGENTPATCHES
 LoadPoints( CP, uPatchNumber+UPATCHOFFSET ); //load the U Tangent control points
 Tan = CalcCubicBezierPatch( CP, U, V );   //calculate the Tangent vector
 LoadPoints( CP, uPatchNumber+VPATCHOFFSET ); //load the V Tangent control points
 Bitan = CalcCubicBezierPatch( CP, U, V );  //calculate the Bitangent vector
#else
 float4 DU = BernsteinPolyD( parms.x, parms.z );// Define the du derivative basis using parameter u 
 Tan = CalcCubicBezierPatch( CP, DU, V ); //Calculate the Tangent from the control points and the derivative of the U weight vector.

 float4 DV = BernsteinPolyD( parms.y, parms.w );// Define the dv derivative basis using parameter v
 Bitan = CalcCubicBezierPatch( CP, U, DV ); //Calculate the Bitangent from the control points and the derivative of the V weight vector.
#endif

 normalize( Tan );
 normalize( Bitan );

// if( uPatchNumber < 0 || uPatchNumber > 367536 )
// if( length( Vert ) < 5.0f )
// {
//  UV = float4( 1, 1, 0, 0 );
// }

 return Vert;
}
#endif//USE_QUADS

/*
//---------------Cubic Interpolation----------------
float3 GetPNQuadVertex( VS_INPUT_TESS In )
{
 float3 Vert = float3(0,0,0);

 // Tricubic Bezier Triangle Patch
 // 3
 // B(u,v,w) = SUM( Bijk * u^i * v^j * w^k * (3!/(i!*j!*k!)) )
 // i=j=k=0
 // w=1-u-v

 //Parametric terms
// float3 I = In.vBarycentric;
// float3 I2 = I * I;
// float3 I3 = I * I2;

 // float a = (1 - u) * (1 - v);
 // float b = u * (1 - v);
 // float c = u * v;
 // float d = (1 - u) * v;
// float u = In.vBarycentric.y + In.vBarycentric.z;
// float v = In.vBarycentric.z + In.vBarycentric.w;
 float u = In.vCartesianCoordinates.x;
 float v = In.vCartesianCoordinates.y;

 // Define the basis using parameter u 
 float BU0 = (1-u) * (1-u) * (1-u);
 float BU1 = 3 * u * (1-u) * (1-u);
 float BU2 = 3 * u * u * (1-u);
 float BU3 = u * u * u;

 // Define the basis using parameter v
 float BV0 = (1-v) * (1-v) * (1-v);
 float BV1 = 3 * v * (1-v) * (1-v);
 float BV2 = 3 * v * v * (1-v);
 float BV3 = v * v * v;

 //coordinates
 float3 P1 = In.Position0;
 float3 P2 = In.Position1;
 float3 P3 = In.Position2;
 float3 P4 = In.Position3;
 float3 N1 = In.Normal0;
 float3 N2 = In.Normal1;
 float3 N3 = In.Normal2;
 float3 N4 = In.Normal3;

 //cubic bezier cooeficients
 //control points (16), C = corners, E = Edges, I = Interior
 float3 P11 = P1; //C
 float3 P21 = (2 * P1 + P2 - dot(P2 - P1, N1) * N1)/3; //E
 float3 P31 = (2 * P2 + P1 - dot(P1 - P2, N2) * N2)/3; //E
 float3 P41 = P2; //C
 float3 P12 = (2 * P1 + P4 - dot(P4 - P1, N1) * N1)/3; //E
 float3 P42 = (2 * P2 + P3 - dot(P3 - P2, N2) * N2)/3; //E
 float3 P13 = (2 * P4 + P1 - dot(P1 - P4, N4) * N4)/3; //E
 float3 P43 = (2 * P3 + P2 - dot(P2 - P3, N3) * N3)/3; //E
 float3 P14 = P4; //C
 float3 P24 = (2 * P4 + P3 - dot(P3 - P4, N4) * N4)/3; //E
 float3 P34 = (2 * P3 + P4 - dot(P4 - P3, N3) * N3)/3; //E
 float3 P44 = P3; //C
*/
/*
 float3 P22 = P1 + ((P21 - P1) + (P12 - P1))/2;
 float3 P32 = P2 + ((P42 - P2) + (P31 - P1))/2;
 float3 P33 = P3 + ((P34 - P3) + (P43 - P3))/2;
 float3 P23 = P4 + ((P13 - P4) + (P24 - P4))/2;
*/
/*
 float3 P23 = P4+(P24 + P13) - (2 * P4); //I
 float3 P33 = P3+(P34 + P43) - (2 * P3); //I
 float3 P22 = P1+(P21 + P12) - (2 * P1); //I
 float3 P32 = P2+(P42 + P31) - (2 * P2); //I

 Vert += BU0 * BV0 * P11; //1
 Vert += BU1 * BV0 * P21; //2
 Vert += BU2 * BV0 * P31; //3
 Vert += BU3 * BV0 * P41; //4
 Vert += BU0 * BV1 * P12; //5
 Vert += BU1 * BV1 * P22; //6
 Vert += BU2 * BV1 * P32; //7
 Vert += BU3 * BV1 * P42; //8
 Vert += BU0 * BV2 * P13; //9
 Vert += BU1 * BV2 * P23; //10
 Vert += BU2 * BV2 * P33; //11
 Vert += BU3 * BV2 * P43; //12
 Vert += BU0 * BV3 * P14;  //13
 Vert += BU1 * BV3 * P24; //14
 Vert += BU2 * BV3 * P34; //15
 Vert += BU3 * BV3 * P44; //16
 return Vert;
}
*/
#ifdef USE_QUADS

#else//USE_QUADS
//---------------Cubic Interpolation----------------
float3 GetPNTriangleVertex( VS_INPUT_TESS In )
{
 float3 Vert = float3(0,0,0);

 // Tricubic Bezier Triangle Patch
 // 3
 // B(u,v,w) = SUM( Bijk * u^i * v^j * w^k * (3!/(i!*j!*k!)) )
 // i=j=k=0
 // w=1-u-v

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
 float3 b210 = (2 * P1 + P2 - s12 * N1); //Tangent Coefficients
 float3 b120 = (2 * P2 + P1 - s21 * N2);
 float3 b021 = (2 * P2 + P3 - s23 * N2);
 float3 b012 = (2 * P3 + P2 - s32 * N3);
 float3 b102 = (2 * P3 + P1 - s31 * N3);
 float3 b201 = (2 * P1 + P3 - s13 * N1);

 float3 b111 = ((b210 + b120 + b021 + b012 + b102 + b201)/2) - (P1 + P2 + P3); //Center Coefficient

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
 // dB(x,y,1-x-y) / dx

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
 float3 b210 = (2 * P1 + P2 - s12 * N1)/3; //Tangent Coefficients
 float3 b120 = (2 * P2 + P1 - s21 * N2)/3;
 float3 b021 = (2 * P2 + P3 - s23 * N2)/3;
 float3 b012 = (2 * P3 + P2 - s32 * N3)/3;
 float3 b102 = (2 * P3 + P1 - s31 * N3)/3;
 float3 b201 = (2 * P1 + P3 - s13 * N1)/3;

 float3 b111 = ((b210 + b120 + b021 + b012 + b102 + b201)/4) - ((P1 + P2 + P3)/6); //Center Coefficient

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
 // dB(x,y,1-x-y) / dy

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
 float3 b210 = (2 * P1 + P2 - s12 * N1)/3; //Tangent Coefficients
 float3 b120 = (2 * P2 + P1 - s21 * N2)/3;
 float3 b021 = (2 * P2 + P3 - s23 * N2)/3;
 float3 b012 = (2 * P3 + P2 - s32 * N3)/3;
 float3 b102 = (2 * P3 + P1 - s31 * N3)/3;
 float3 b201 = (2 * P1 + P3 - s13 * N1)/3;

 float3 b111 = ((b210 + b120 + b021 + b012 + b102 + b201)/4) - ((P1 + P2 + P3)/6); //Center Coefficient

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
 // 2
 // N(u,v,w) = SUM( Vijk * u^i * v^j * w^k )
 // i=j=k=0
 // w=1-u-v

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
 Norm *= sign(dot(N,Norm)); //flip if opposite of interpolated normal

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

 // Norm += cross( E2, E1 );
 // Norm += cross( E3, E2 );
 // Norm += cross( E1, E3 );

 float3 V = GetLinearTriangleVertex( In ).xyz;
 float3 N = GetLinearTriangleNormal( In );

 float3 DV = Center - V;

 // Norm = N + (V - Center);
 Norm = N + (DV * sign(dot(DV,N)));

 return normalize( Norm );
}
/*
float3 GetNormalSelect( VS_INPUT_TESS In, float3 in_Pos )
{
 float3 Norm = float3(0,0,0);
 if (g_normalMethod == 0)
  Norm = GetLinearTriangleNormal( In ); //linear interpolation
 else if (g_normalMethod == 1)
  Norm = GetPNTriangleNormal( In ); //triquadratic interpolation
 else if (g_normalMethod == 2)
  Norm = GetAverageTriangleNormal( In, in_Pos ); //Average from interpolated normal and vertex position
 else if (g_normalMethod == 3)
  Norm = GetTrueTriangleNormal( In ); //tricubic surface normal vector (using tan and bitan)
 return Norm;
}
float3 GetTangentSelect( VS_INPUT_TESS In, float3 in_Pos )
{
 float3 Tangent = float3(0,0,0);
 if (g_normalMethod == 0)
  Tangent = GetPNTriangleTangent( In ); //triquadratic interpolation
 else 
  Tangent = GetLinearTriangleTangent( In ); //linear interpolation
 return Tangent;
}
float3 GetBitangentSelect( VS_INPUT_TESS In, float3 in_Pos )
{
 float3 BiTangent = float3(0,0,0);
 if (g_normalMethod == 0)
  BiTangent = GetPNTriangleBitangent( In ); //triquadratic interpolation
 else 
  BiTangent = GetLinearTriangleBitangent( In ); //linear interpolation
 return BiTangent;
}
*/

#endif//USE_QUADS

float2 SobelFilter( sampler2D HeightMap, float4 texCoord, float2 TextureSize )
{
 float2 off = 1.0 / TextureSize;
 float lod = g_DisplacementBlur*10.0f;

 // Take all neighbor samples
 float s00 = tex2Dlod(HeightMap, texCoord + float4(-off.x, -off.y, 0, lod)).r;
 float s01 = tex2Dlod(HeightMap, texCoord + float4( 0, -off.y, 0, lod)).r;
 float s02 = tex2Dlod(HeightMap, texCoord + float4( off.x, -off.y, 0, lod)).r;

 float s10 = tex2Dlod(HeightMap, texCoord + float4(-off.x, 0, 0, lod)).r;
 float s12 = tex2Dlod(HeightMap, texCoord + float4( off.x, 0, 0, lod)).r;

 float s20 = tex2Dlod(HeightMap, texCoord + float4(-off.x, off.y, 0, lod)).r;
 float s21 = tex2Dlod(HeightMap, texCoord + float4( 0, off.y, 0, lod)).r;
 float s22 = tex2Dlod(HeightMap, texCoord + float4( off.x, off.y, 0, lod)).r;

 // Slope in X direction
 float sobelX = s00 + 2 * s10 + s20 - s02 - 2 * s12 - s22;
 // Slope in Y direction
 float sobelY = s00 + 2 * s01 + s02 - s20 - 2 * s21 - s22;

 return float2( sobelX, sobelY );
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
float3 NormalFromDisplacement( float4 texCoord, float2 UVScale )
{
 float2 off = 1.0 / g_DisplacementMapSize;
 float lod = g_DisplacementBlur*10.0f;

 float2 distance = UVScale * off;

 float ctr = (tex2Dlod( displacementSampler, texCoord + float4(0, 0, 0, lod)).r - g_DisplacementBias) * g_DisplacementScale;
 float ul = (tex2Dlod( displacementSampler, texCoord + float4(-off.x, 0, 0, lod)).r - g_DisplacementBias) * g_DisplacementScale;
 float ur = (tex2Dlod( displacementSampler, texCoord + float4( off.x, 0, 0, lod)).r - g_DisplacementBias) * g_DisplacementScale;
 float vt = (tex2Dlod( displacementSampler, texCoord + float4( 0, -off.x, 0, lod)).r - g_DisplacementBias) * g_DisplacementScale;
 float vb = (tex2Dlod( displacementSampler, texCoord + float4( 0, off.x, 0, lod)).r - g_DisplacementBias) * g_DisplacementScale;

 // Average normals four neighboring quads:
 // 
 // V2
 // U2 X U 
 // V 

 float3 vUX = float3( distance.x, 0, ur - ctr ) ;
 float3 vVX = float3( 0, -distance.y, vt - ctr ) ;
 float3 vU2X = float3( -distance.x, 0, ul - ctr ) ;
 float3 vV2X = float3( 0, distance.y, vb - ctr ) ;

 return -normalize( normalize( cross( vUX, vVX ) ) + 
  normalize( cross( vVX, vU2X ) ) +
  normalize( cross( vU2X, vV2X ) ) +
  normalize( cross( vV2X, vUX ) ) );
}

//--------------------------------------------------------------------------------------------------------------------------------------------
float3x3 MakeFromToRotationMatrixFast ( float3 vFrom, float3 vTo )
{
 float3x3 mResult;

 float3 axis = cross( vFrom, vTo );  //axis of rotation
 float ang = dot( vFrom, vTo );   //angle of rotation
 float fH = 1.0 / (1.0 + ang);

 mResult[0][0] = ang + fH * axis.x * axis.x;
 mResult[1][0] = fH * axis.x * axis.y + axis.z;
 mResult[2][0] = fH * axis.x * axis.z - axis.y;

 mResult[0][1] = fH * axis.x * axis.y - axis.z;
 mResult[1][1] = ang + fH * axis.y * axis.y;
 mResult[2][1] = fH * axis.y * axis.z + axis.x;

 mResult[0][2] = fH * axis.x * axis.z + axis.y;
 mResult[1][2] = fH * axis.y * axis.z - axis.x;
 mResult[2][2] = ang + fH * axis.z * axis.z;

 return mResult;

} // End of MakeFromToRotationMatrixFast(..)


float3 BarycentricFromUV( VS_INPUT_TESS In, float2 UV )
{
 float3 Barycenter;

 float det = ((In.UV0.x * In.UV1.y) - (In.UV0.x * In.UV2.y) - (In.UV1.x * In.UV0.y) + (In.UV1.x * In.UV2.y) + (In.UV2.x * In.UV0.y) - (In.UV2.x * In.UV1.y));

 Barycenter.x = ((UV.x * In.UV1.y) - (UV.x * In.UV2.y) - (In.UV1.x * UV.y) + (In.UV1.x * In.UV2.y) + (In.UV2.x * UV.y) - (In.UV2.x * In.UV1.y)) / det;
 Barycenter.y = ((In.UV0.x * UV.y) - (In.UV0.x * In.UV2.y) - (UV.x * In.UV0.y) + (UV.x * In.UV2.y) + (In.UV2.x * In.UV0.y) - (In.UV2.x * UV.y)) / det;
 Barycenter.z = 1 - Barycenter.x - Barycenter.y;

 return Barycenter;
}

float3 DisplaceNormal( VS_INPUT_TESS In, float3 Center, float3 Pos, float2 UV, float3 Norm )
{
 float2 fDelta = 1.0f / g_DisplacementMapSize;

 // Average normals four neighboring quads:
 // 
 // T
 //
 // L C R 
 //
 // B 

 //right vertex
 float2 RUV = UV + float2( fDelta.x, 0 );
 float3 RBarycenter = BarycentricFromUV( In, RUV );

 float3 RPos = PositionTess( In, RBarycenter ).xyz;
 float3 RNorm = NormalTess( In, RBarycenter );

 float3 Right = DisplaceVertex( RPos, RNorm, RUV );

 //left vertex
 float2 LUV = UV + float2( -fDelta.x, 0 );
 float3 LBarycenter = BarycentricFromUV( In, LUV );

 float3 LPos = PositionTess( In, LBarycenter ).xyz;
 float3 LNorm = NormalTess( In, LBarycenter );

 float3 Left = DisplaceVertex( LPos, LNorm, LUV );

 //top vertex
 float2 TUV = UV + float2( 0, -fDelta.y );
 float3 TBarycenter = BarycentricFromUV( In, TUV );

 float3 TPos = PositionTess( In, TBarycenter ).xyz;
 float3 TNorm = NormalTess( In, TBarycenter );

 float3 Top = DisplaceVertex( TPos, TNorm, TUV );

 //bottom vertex
 float2 BUV = UV + float2( 0, fDelta.y );
 float3 BBarycenter = BarycentricFromUV( In, BUV );

 float3 BPos = PositionTess( In, BBarycenter ).xyz;
 float3 BNorm = NormalTess( In, BBarycenter );

 float3 Bottom = DisplaceVertex( BPos, BNorm, BUV );

 //calculate vectors
 float3 RVec = Right - Center;
 float3 LVec = Left - Center;
 float3 TVec = Top - Center;
 float3 BVec = Bottom - Center;

 //average normals
 return normalize( Norm +
  cross( RVec, TVec ) + 
  cross( TVec, LVec ) +
  cross( LVec, BVec ) +
  cross( BVec, RVec ) );
}

float3 DisplaceNormalOpt( VS_INPUT_TESS In, float3 Center, float4 UV )
{
 float2 UVScale;
 float2 fDelta = 1.0f / g_DisplacementMapSize;

 float3 OffsetBarycenterU = BarycentricFromUV( In, UV.xy + float2( fDelta.x, 0) );
 float3 OffsetBarycenterV = BarycentricFromUV( In, UV.xy + float2( 0, fDelta.y) );

 UVScale.x = length(OffsetBarycenterU - Center);
 UVScale.y = length(OffsetBarycenterV - Center);

 return NormalFromDisplacement( UV, UVScale );
}

void Tessellate(VS_INPUT_TESS In, out STANDARD_VERTEX Out, in bool bVertexDisplace = false )
{
#ifdef USE_QUADS
 float3 Tan;
 float3 Bitan;
// Out.Position = GetPNQuadVertexLU2( In );
// Out.Position = GetQuadPatchLUVertex( In, Tan, Bitan ) + In.Position0 + In.Position1 + In.Position2 + In.Position3;

 Out.UV = float4(GetLinearQuadTexture( In ),0,0);
 Out.Position = GetQuadPatchLUVertex( In, Tan, Bitan, Out.UV );
// Out.Position = GetLinearQuadVertexLU( In );
// Out.Position = GetLinearQuadVertex( In );
// Out.Position = GetPNQuadVertex( In );
 // Out.Normal = GetNormalSelect( In, Out.Position );
// Out.Normal = GetLinearQuadNormal( In );
// Out.UV = GetLinearQuadTexture( In );
 // Out.T = GetTangentSelect( In, Out.Position );
 // Out.B = GetBitangentSelect( In, Out.Position );
// Out.T = GetLinearQuadNormal( In );
// Out.B = GetLinearQuadNormal( In );
// Out.UV = In.UV0;

// if( length( Out.Position ) < 5.0f )
// {
//  Out.UV = float4( 1, 1, 0, 0 );
// }


// Out.UV = In.vCartesianCoordinates;
// Out.UV = CalculatePatchParametricCoordinates( In.vCartesianCoordinates, In.vIndices );
// Out.UV = float4(GetLinearQuadVertex( In ),0.0f);
 Out.Normal = cross( Tan, Bitan );
 Out.T = Tan;
 Out.B = Bitan;
// Out.B = SampleVertexData( In.vIndices.x*4 ).rgb; //scale index by 4 to allow 16 float4 per quad superprimative

#else//USE_QUADS

 float4 PosTess = GetLinearTriangleVertex( In );
 Out.Position = PosTess.xyz;
// Out.Position = GetPNTriangleVertex( In );
// Out.Normal = GetNormalSelect( In, Out.Position );
 Out.Normal = GetLinearTriangleNormal( In );
 Out.UV = GetLinearTriangleTexture( In );
// Out.T = GetTangentSelect( In, Out.Position );
// Out.B = GetBitangentSelect( In, Out.Position );
 Out.T = GetLinearTriangleTangent( In );
 Out.B = GetLinearTriangleBitangent( In );

#endif//USE_QUADS

 float4 tex2 = mul(g_uvTransform, Out.UV );
#ifdef APPLY_DISPLACEMENT
 Out.Position = DisplaceVertex( Out.Position, Out.Normal, tex2.xy );
#endif

#ifdef CALCULATE_BUMP_NORMAL
/*
 float2 dxdy = SobelFilter( displacementSampler, tex2, g_DisplacementMapSize ) * g_DisplacementScale * 16.0f;
 float3 TanNormal = normalize(float3( dxdy.x, dxdy.y, 1 ));

 float3 WorldTanX, WorldTanY, WorldTanZ;
 TangentToWorldSpace( g_world, Out.T, Out.B, Out.Normal, WorldTanX, WorldTanY, WorldTanZ);
 Out.Normal = normalize(float3(dot(TanNormal,WorldTanX), dot(TanNormal,WorldTanY), dot(TanNormal,WorldTanZ)));
*/
/*
 // Compute normal for displaced surface, in tangent space, from the displacement map directrly 
 float3 vDisplacedNormal = TangentSpaceFromDisplacement( tex2 );

 // These 'CalculateNormal' functions assume a tangent space where:
 // - Y+ is up
 // - X+ is the tangent in the U direction
 // - Z- is the tangent in the V direction (binormal)

 // Convert normal to world space (note that the tangent to world transform is different than usual)
 float3x3 mTangent = { Out.T, Out.Normal, -Out.B };
// float3x3 mTangent = { Out.T, -Out.B, Out.Normal };

 // Rotate tangent frame to line up to the bumped normal (for subsequent displacements or normal-mapping)
 float3x3 mBumpRotation = MakeFromToRotationMatrixFast( Out.Normal, vDisplacedNormal );

 Out.Normal = mul( vDisplacedNormal, mTangent );
 Out.T = mul( mBumpRotation, Out.T );
 Out.B = mul( mBumpRotation, Out.B );
*/
// Out.Normal = DisplaceNormal( In, Out.Position, PosTess, tex2, Out.Normal );
 if( bVertexDisplace )
 {
  float3 TanNormal = NormalFromDisplacement( tex2, g_ObjectUVScale );
//  float3 TanNormal = DisplaceNormalOpt( In, PosTess, tex2 );
  // These 'CalculateNormal' functions assume a tangent space where:
  // - Y+ is up
  // - X+ is the tangent in the U direction
  // - Z- is the tangent in the V direction (binormal)

  // Convert normal to world space 
  float3x3 TanMat = {Out.T, Out.B, Out.Normal};

  // Rotate tangent frame to line up to the bumped normal (for subsequent displacements or normal-mapping)
  float3x3 mBumpRotation = MakeFromToRotationMatrixFast( normalize(Out.Normal), TanNormal );

  Out.Normal = mul( TanNormal, TanMat );
  Out.T = mul( mBumpRotation, Out.T );
  Out.B = mul( mBumpRotation, Out.B );


/*
  float3 TanNormal = NormalFromDisplacement( V.TexCoord0, g_ObjectUVScale );


  // Rotate tangent frame to line up to the bumped normal (for subsequent displacements or normal-mapping)
  float3x3 mBumpRotation = MakeFromToRotationMatrixFast( normalize(V.WorldTan.Z), TanNormal );

  // Convert normal to world space
  V.WorldTan.Z = mul( TanNormal, TanMat );
  //transform tangent frame
  V.WorldTan.X = mul( mBumpRotation, V.WorldTan.X );
  V.WorldTan.Y = mul( mBumpRotation, V.WorldTan.Y );
*/
 }
#endif
}

float3 TangentNormaltoWorld( in TANGENT_MATRIX mat, in float3 TanNormal, in float vFace )
{
 float3x3 TanMat = {mat.X, mat.Y, mat.Z};
 // Convert normal to world space
 float3 worldNormal = mul( TanNormal, TanMat );

 if( g_bDoubleSided && vFace > 0)
 {
  worldNormal = -worldNormal;
 }

 return normalize(worldNormal);
}

/*
float3 GetNormal( in TANGENT_VERTEX V, in float vFace )
{
 float3 TanNormal;
 if( g_hasDisplacementMap )
 {
  float2 dP = float2(length(ddx(V.WorldPos)), length(ddy(V.WorldPos)));
  float dPdU = dP.x / length(ddx(V.TexCoord0));
  float dPdV = dP.y / length(ddy(V.TexCoord0));
//  float dPdU = length(float2(dP.x/ddx(V.TexCoord0.x), dP.y/ddy(V.TexCoord0.x)));
//  float dPdV = length(float2(dP.x/ddx(V.TexCoord0.y), dP.y/ddy(V.TexCoord0.y)));

  TanNormal = NormalFromDisplacement( V.TexCoord0, float2( dPdU, dPdV ));
 }
 else
 {
  TanNormal = float3(0,0,1);
 }

 return TangentNormaltoWorld( V.WorldTan, TanNormal, vFace );
}
*/
float3 GetNormal( in TANGENT_VERTEX V, in float vFace )
{
 float3 worldNormal = V.WorldTan.Z;

 if( g_bDoubleSided && vFace > 0)
 {
  worldNormal = -worldNormal;
 }

 return normalize(worldNormal);
}

float3 GetBumpNormal( in TANGENT_VERTEX V, in bool hasBump, in sampler2D bumpMap, float bumpMapScale, in float vFace )
{
 if( hasBump )
 {
  float3 TanNormal = ((tex2D(bumpMap, V.TexCoord0.xy).xyz * 2) - 1) * float3(bumpMapScale,bumpMapScale,1);
  return TangentNormaltoWorld( V.WorldTan, TanNormal, vFace );
 }
 return GetNormal( V, vFace );
}

float3 Get2BumpNormal( in TANGENT_VERTEX V,
      in bool hasBump, in sampler2D bumpMap, float bumpMapScale,
      in bool hasMicroBump, in sampler2D microBumpMap, float microBumpMapScale,
      in float vFace )
{
 if( hasMicroBump || hasBump )
 {
  float3 bumpNormal = float3(0,0,1);
  if( hasMicroBump )
  {
   bumpNormal += (tex2D(microBumpMap,V.TexCoord0.xy*microBumpMapScale).xyz * 2) - 1;
  }
  if( hasBump )
  {
   bumpNormal += ((tex2D(bumpMap, V.TexCoord0.xy).xyz * 2) - 1) * float3(bumpMapScale,bumpMapScale,1);
  }
  return TangentNormaltoWorld( V.WorldTan, bumpNormal, vFace );
 }
 return GetNormal( V, vFace );
}

//-----------------------------------------------------
// Alters the tangent basis matrix from a displacement map
// (Pixel Shader only and Tessellated only)
//-----------------------------------------------------

void TangentDisplace( inout TANGENT_VERTEX V )
{
 if( g_hasDisplacementMap )
 {
//  float2 dP = float2(length(ddx(V.WorldPos)), length(ddy(V.WorldPos)));
//  float dPdU = dP.x / length(ddx(V.TexCoord0));
//  float dPdV = dP.y / length(ddy(V.TexCoord0));
//  float dPdU = length(float2(dP.x/ddx(V.TexCoord0.x), dP.y/ddy(V.TexCoord0.x)));
//  float dPdV = length(float2(dP.x/ddx(V.TexCoord0.y), dP.y/ddy(V.TexCoord0.y)));

//  float3 TanNormal = NormalFromDisplacement( V.TexCoord0, float2( 10, 10 ));
//  float3 TanNormal = NormalFromDisplacement( V.TexCoord0, float2( dPdU, dPdV ));
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
}

/*********** eof **************/
/*****************************************************************************
** Tessellate.h
**
** ATI Hardware Tessellation functions
**
** Extra Large Technology
** Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

/*********** structures ******/
struct VS_INPUT_SKINNING
{
 float3 Position : POSITION;
 float3 Normal : NORMAL;
 float4 UV  : TEXCOORD0;
 float3 T  : TANGENT;
 float3 B  : BINORMAL;
 float4 Bones : TEXCOORD1;
 float4 Weights : TEXCOORD2;
};

/*********** data ******/
#ifndef MATRIX_PALETTE_SIZE_DEFAULT
#define MATRIX_PALETTE_SIZE_DEFAULT 50
#endif
float4x4 g_BonePalette[MATRIX_PALETTE_SIZE_DEFAULT] : BONES;

/*********** functions ******/
/*********** support functions ******/
void Skin(in VS_INPUT_SKINNING In, 
   out STANDARD_VERTEX Out)
{
// maximum of 4 influencing transforms:

 // if Bones were passed in as a float1 instead of float4, 
 // then we can break it into an array of 4 bytes like this:
 // int4 iBones = D3DCOLORtoUBYTE4(Bones);

 float4x4 finalMatrix;
 finalMatrix = In.Weights.x * g_BonePalette[In.Bones.x];
 finalMatrix += In.Weights.y * g_BonePalette[In.Bones.y];
 finalMatrix += In.Weights.z * g_BonePalette[In.Bones.z];
 finalMatrix += In.Weights.w * g_BonePalette[In.Bones.w];
 
 Out.Position = mul(float4(In.Position,1), finalMatrix).xyz;
 Out.Normal  = mul(In.Normal, (float3x3)finalMatrix);
 Out.T   = mul(In.T, (float3x3)finalMatrix);
 Out.B   = mul(In.B, (float3x3)finalMatrix);
 Out.UV = In.UV;//?

 // In the sample file, this should do a rigid bend of the cylinder
 // as if the full cylinder was influenced only by the second bone...
// float4 final_pos = mul(Pos, g_BonePalette[1]);
// return final_pos;
 
 // Combining the transformed positions
//  float4 final_pos = Weights.x * mul(Pos, g_BonePalette[Bones.x]);
//  final_pos += Weights.y * mul(Pos, g_BonePalette[Bones.y]);
//  final_pos += Weights.z * mul(Pos, g_BonePalette[Bones.z]);
//  final_pos += Weights.w * mul(Pos, g_BonePalette[Bones.w]);
//  return final_pos;
 
}

/*********** eof **************/
/*****************************************************************************
** Lighting.h
**
** Support functions for .fx shaders
**
** StudioGPU
** Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

//define this to disable PCSS (shaders load 10x faster)
//#define SIMPLE_SHADOWS


/*********** support data ******/
struct LightInfo
{
 float4 Pos;
 float4 Diffuse;
 float4 Specular;
 float4 Falloff;
 float4 ConeInfo; /* x,y,z are normalized direction, w is cos(ConeAngle) */
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
};
struct IncidentLight
{
 // direction, unnormalized
 float3 L;
 // color
 float3 Cld;
 float3 Cls;
};

LightInfo g_lightArray[8] : LightArray;
LightInfo g_lightInfo : LightInfo;
ProjLightInfo g_projLight : ProjLightInfo;

bool g_bProjLt = false;
bool g_bHasProjMap : HasProjectedTexture = false;
bool g_bHasShadowMap : HasShadowMap = false;

#ifdef SIMPLE_SHADOWS
#define BLOCKER_SAMPLES_LOW 2
#define SHADOW_SAMPLES_LOW 2

#define BLOCKER_SAMPLES_MED 2
#define SHADOW_SAMPLES_MED 2

#define BLOCKER_SAMPLES_HIGH 2
#define SHADOW_SAMPLES_HIGH 2

#define BLOCKER_SAMPLES_VERY_HIGH 2
#define SHADOW_SAMPLES_VERY_HIGH 2

#else//SIMPLE_SHADOWS

#define BLOCKER_SAMPLES_LOW 5
#define SHADOW_SAMPLES_LOW 5

#define BLOCKER_SAMPLES_MED 7
#define SHADOW_SAMPLES_MED 7

#define BLOCKER_SAMPLES_HIGH 9
#define SHADOW_SAMPLES_HIGH 9

#define BLOCKER_SAMPLES_VERY_HIGH 15
#define SHADOW_SAMPLES_VERY_HIGH 15

#endif//SIMPLE_SHADOWS

#define POISSONTEX 256.0

texture2D projLightMap : ProjLightTexture;
sampler projSampler = sampler_state // glow, projLight
{
 Texture = (projLightMap);
 MipFilter = LINEAR;
 MinFilter = LINEAR;
 MagFilter = LINEAR;
 AddressU = Border;
 AddressV = Border;
 AddressW = Border;
};
texture2D projShadowMap : ProjShadowMap;
sampler shadowMapSampler = sampler_state // glow, projLight
{
 Texture = (projShadowMap);
 MipFilter = LINEAR;
 MinFilter = LINEAR;
 MagFilter = LINEAR;
 AddressU = Clamp;
 AddressV = Clamp;
 AddressW = Clamp;
};

texture2D g_Poisson;
sampler2D poissonSampler = sampler_state
{
 Texture = (g_Poisson);
 MipFilter = POINT;
 MinFilter = POINT;
 MagFilter = POINT;
 AddressU = clamp;
 AddressV = clamp;
 AddressW = clamp;
};

/*********** support functions ******/
//--------------------------------------------------------------------
// attenuation() - calculate a factor to modify light intensity based on distance
//--------------------------------------------------------------------// 
float attenuation(float3 Pw,  // Position of vertex in world coords
     LightInfo light)
{
 float atten = 1;
 if (light.Pos.w > 0.5) // only point lights (w == 1)
 {
  float d = distance(Pw, light.Pos.xyz);
  if (d > light.Falloff.w)
  {
   atten = 0.0f;
  }
  else
  {
   atten = 1 / (light.Falloff.x + light.Falloff.y * d + light.Falloff.z * d * d);
  }
 }

 return atten;
}

//--------------------------------------------------------------------
// diffuse_contrib()
//--------------------------------------------------------------------
void diffuse_contrib(LightInfo lightInfo,
      float3 Pw,   // Position of vertex in world coords
      float3 Nn,   // Normalize vertex normal
      float3 V,   // Eye position, world - vertex position
      float shininess, // Specular power

      out float4 diffContrib,
      out float4 specContrib)
{
 float atten = attenuation(Pw, lightInfo);
 float3 Ln = normalize(lightInfo.Pos.xyz - mul(Pw, lightInfo.Pos.w));

 float ldn = (dot(Ln,Nn));
 float diffComp = saturate(ldn) * atten;

 float3 H = normalize(Ln + V);
 float hdn = saturate(dot(Nn, H));
 float specComp = pow(hdn, max(0.001f, shininess)) * atten;

 // Only approximated Projected lights have
 // the cone angle set, all others use 0 for w
 // (Note: w==0 could also occur when the angle of the
 // projected light is 180 degrees)
 if (lightInfo.ConeInfo.w > 0)
 {
  // Cone angle computation
  // ConeInfo.xyz is negated direction of spot light,
  // ConeInfo.w is cos(0.5 * angle of light)
  // If dot of light direction and vector to light is less
  // than cone angle, remove the light's contribution
  float cos_cone = (dot(Ln, lightInfo.ConeInfo.xyz));
  if (cos_cone < lightInfo.ConeInfo.w)
  {
   diffComp = 0.0;
   specComp = 0.0;
  }
 }

 diffContrib = ((diffComp) * (lightInfo.Diffuse));
 //diffContrib.w = 1.0;
 diffContrib.w = 0.0;

 //diffContrib = float4(0,0,0,1);
 //diffContrib = CheckNan4(diffContrib);


 //if (diffComp <= 0) specComp = 0;
 specContrib = ((specComp) * (lightInfo.Specular));
 specContrib.w = 0.0;

 //specContrib = float4(0,0,0, 0);
 //specContrib = CheckNan4(specContrib);

 //return float4(diffComp,diffComp,diffComp,1);
}

/*********** shadow mapping **********/
// simple averaging with grid of samples about point:
// fTexelSize is half filter width (samples are spread from 
// -fTexelSize to +fTexelSize around ProjTexCoord)
// numSamples must be > 1! odd numbers of samples will 
// sample the original point, while even numbers will miss it.
//--------------------------------------------------------------------
// SampleShadowMap() - deprecated
//--------------------------------------------------------------------
float4 SampleShadowMap(sampler2D ProjShadowMap, float4 ProjTexCoord, 
      uniform int samples, float fTexelSize = 1.0f/24.0f)
{
 // divide filter width by number of samples to use
 float fShadowTerm = 0.0f; 
 float fCurShadowTerm;

 // iterate through search region and add up depth values
 for (int i=0; i<samples*samples; i++) 
 {
  float2 offset = tex2Dlod(poissonSampler, float4((float)(i+0.5)/POISSONTEX,0,0,0)).xy * (fTexelSize * ProjTexCoord.w);
  fCurShadowTerm = tex2Dproj( ProjShadowMap, ProjTexCoord+float4(offset,0,0)).x;
  fShadowTerm += fCurShadowTerm;
 }
 fShadowTerm /= (samples*samples);

 fShadowTerm = lerp(1-g_projLight.ShadowIntensity, 1, fShadowTerm);

 return fShadowTerm;
}

//--------------------------------------------------------------------
// GetPoissonVal() - PCSS helper function
//--------------------------------------------------------------------
float2 GetPoissonVal(int i)
{
 return tex2Dlod(poissonSampler, float4((float)(i+0.5)/POISSONTEX,0,0,0)).xy;
}

//--------------------------------------------------------------------
// PenumbraSize() - PCSS helper function
//--------------------------------------------------------------------
float PenumbraSize(float zReceiver, float zBlocker) //Parallel plane estimation
{
 return (zReceiver - zBlocker) / zBlocker;
}

//--------------------------------------------------------------------
// FindBlocker() - PCSS helper function
//--------------------------------------------------------------------
void FindBlocker(out float avgBlockerDepth,
     out float numBlockers,
     float2 uv, float zReceiver,
     uniform sampler2D ShadowMap, 
     int nBlockerSamples, float lightSizeUV)
{
 // This uses similar triangles to compute what area of the shadow map we should search
 float searchWidth = lightSizeUV * (zReceiver - g_projLight.NearFar.x) / zReceiver;
 float blockerSum = 0;
 numBlockers = 0;
 float2 offset;
 float shadowMapDepth;
 for( int i = 0; i < nBlockerSamples; ++i )
 {
  for( int j = 0; j < nBlockerSamples; ++j )
  {
   offset = GetPoissonVal(i*nBlockerSamples+j) * searchWidth;
   shadowMapDepth = tex2Dlod(ShadowMap, float4(uv + offset,0,0)).x;
 
   if ( shadowMapDepth < zReceiver ) {
    blockerSum += shadowMapDepth;
    numBlockers++;
   }
  }
 }
 avgBlockerDepth = blockerSum / numBlockers;
}

//--------------------------------------------------------------------
// PCF_Filter() - PCSS helper function
//--------------------------------------------------------------------
float PCF_Filter( float2 uv, float zReceiver, float filterRadiusUV,
     uniform sampler2D ShadowMap, int nShadowSamples)
{
 float sum = 0.0f;
 float2 offset;
 float shadowMapDepth;
 for( int i = 0; i < nShadowSamples; ++i )
 { 
  for( int j = 0; j < nShadowSamples; ++j )
  {
   offset = GetPoissonVal(i*nShadowSamples+j) * filterRadiusUV;
   shadowMapDepth = tex2Dlod(ShadowMap, float4(uv + offset,0,0)).x;
   sum += (zReceiver <= shadowMapDepth) ? 1 : 0;
  }
 }
 return sum / (nShadowSamples*nShadowSamples);
}

//--------------------------------------------------------------------
// PCSS() - percentage-closer soft shadows
//--------------------------------------------------------------------
float PCSS( uniform sampler2D ProjShadowMap, float4 ProjTexCoord, 
   uniform int nBlockerSamples, uniform int nShadowSamples)
{
 float2 uv = ProjTexCoord.xy/ProjTexCoord.w;
 float zReceiver = ProjTexCoord.z/ProjTexCoord.w; // Assumed to be eye-space z in this code
 float lightSizeUV = g_projLight.LightSize / g_projLight.Scale;

 // STEP 1: blocker search
 float avgBlockerDepth = 0;
 float numBlockers = 0;
 FindBlocker( avgBlockerDepth, numBlockers, uv, zReceiver, ProjShadowMap, nBlockerSamples, lightSizeUV );

 // There are no occluders so early out (this saves filtering)
 if( numBlockers < 1 )  
  return 1.0f;

 // STEP 2: penumbra size
 float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
 float filterRadiusUV = penumbraRatio * lightSizeUV * g_projLight.NearFar.x / zReceiver;
 float interpolated = lerp(g_projLight.LightSize,filterRadiusUV,g_projLight.PCSSAdjust);
 
 // STEP 3: filtering
 float shadowed = PCF_Filter( uv, zReceiver, interpolated, ProjShadowMap, nShadowSamples );

 // STEP 4: shadow intensity
 shadowed = lerp(1-g_projLight.ShadowIntensity, 1, shadowed);
 
 return shadowed;
}

// Illuminate* computes how much light arrives at point Ps 
//--------------------------------------------------------------------
// IlluminatePointLight() - simple point light
//--------------------------------------------------------------------
void IlluminatePointLight(in float3 Ps, in LightInfo light, out IncidentLight OUT)
{
 OUT.L = light.Pos.xyz - Ps;

 //float d = length(OUT.L);
 //float atten = 1 / (light.Falloff.x + light.Falloff.y * d + light.Falloff.z * d * d);
 float atten = attenuation(Ps, light);

 OUT.Cld = atten * light.Diffuse.rgb;
 OUT.Cls = atten * light.Specular.rgb;
}

//--------------------------------------------------------------------
// IlluminateProjLight() - projected light, no shadow
//--------------------------------------------------------------------
void IlluminateProjLight(in float3 Ps, in LightInfo light, in ProjLightInfo projLight, 
       in sampler2D ProjTextureMap, out IncidentLight OUT)
{
 OUT.L = projLight.Pos.xyz - Ps;

 OUT.Cld = OUT.Cls = float3(0,0,0);
 float4 projTexCoord = mul(projLight.Matrix, float4(Ps,1));
 if (projTexCoord.z >= 0)
 {
  float4 tex_col = 1;
  if (g_bHasProjMap)
   tex_col = tex2Dproj(ProjTextureMap, projTexCoord); 
  else
  {
   float2 uv = projTexCoord.xy/projTexCoord.w;
   uv = step(0, uv) * step(uv, 1);
   tex_col = uv.x * uv.y;
  }

  //float d = length(OUT.L);
  //float atten = 1 / (light.Falloff.x + light.Falloff.y * d + light.Falloff.z * d * d);
  float atten = attenuation(Ps, light);

  float3 modifier = atten * tex_col.rgb;
  OUT.Cld = modifier * light.Diffuse.rgb;
  OUT.Cls = modifier * light.Specular.rgb;
 }
}

//--------------------------------------------------------------------
// IlluminateProjLight() - projected light with shadow
//--------------------------------------------------------------------
void IlluminateProjLight(in float3 Ps, in LightInfo light, in ProjLightInfo projLight, 
       in sampler2D ProjTextureMap, in sampler2D ProjShadowMap, 
       in uniform int nBlockerSamples, in uniform int nShadowSamples,
       out IncidentLight OUT)
{
 OUT.L = projLight.Pos.xyz - Ps;

 OUT.Cld = OUT.Cls = float3(0,0,0);
 float4 projTexCoord = mul(projLight.Matrix, float4(Ps,1));
 if (projTexCoord.z >= 0)
 {
  float4 tex_col = 1;
  if (g_bHasProjMap)
   tex_col = tex2Dproj(ProjTextureMap, projTexCoord); 
  else
  {
   float2 uv = projTexCoord.xy/projTexCoord.w;
   uv = step(0, uv) * step(uv, 1);
   tex_col = uv.x * uv.y;
  }

  //float d = length(OUT.L);
  //float atten = 1 / (light.Falloff.x + light.Falloff.y * d + light.Falloff.z * d * d);
  float atten = attenuation(Ps, light);

  float4 shadowCoeff = 1;
  if (g_bHasShadowMap)
  {
   shadowCoeff = PCSS(ProjShadowMap, projTexCoord, nBlockerSamples, nShadowSamples);
   //shadowCoeff = SampleShadowMap(ProjShadowMap, projTexCoord, nShadowSamples, g_projLight.LightSize);
  }   

  // float3 modifier = atten * tex_col.rgb * shadowCoeff.rgb;
  // OUT.Cld = modifier * light.Diffuse.rgb;
  // OUT.Cls = modifier * light.Specular.rgb;
  OUT.Cld = lerp(projLight.ShadowColor.rgb, atten * tex_col.rgb * light.Diffuse.rgb, shadowCoeff.rgb);
  OUT.Cls = lerp(projLight.ShadowColor.rgb, atten * tex_col.rgb * light.Specular.rgb, shadowCoeff.rgb);
 }
}

//--------------------------------------------------------------------
// IlluminateImageBasedLight()
//--------------------------------------------------------------------
void IlluminateImageBasedLight(in float3 Ps, in float3 Ns, 
        in float3 I,
        in float diffuseFactor,
        in bool bHasDiffuseEnvMap,
        in samplerCUBE DiffuseEnvMap,
        in float diffuseEnvAngle,
        in float specularFactor,
        in bool bHasSpecularEnvMap,
        in samplerCUBE SpecularEnvMap,
        in float specularEnvAngle,
        out IncidentLight OUT)
{
 // light incident is equal to opposite of normal,
 // for environment lights.
 OUT.L = -Ns;
 OUT.Cld = OUT.Cls = float3(0,0,0);

 // sample diffuse using normal vector
 if (diffuseFactor > 0)
 {
  if (bHasDiffuseEnvMap)
  {
   float3 Nr = rotateAboutY(Ns, diffuseEnvAngle);
   OUT.Cld = texCUBE(DiffuseEnvMap, Nr).rgb;
  }
 }

 // sample specular using reflection vector
 if (specularFactor > 0)
 {
  if (bHasSpecularEnvMap)
  {
   float3 Nr = rotateAboutY(Ns, specularEnvAngle);

   // I points from eyepos to Ps.
   // world eye dir points FROM shade point TO eye pos so use -I
   // do world space reflection
   float NdotI = dot(-I, Nr);
   //R = 2*N*(L.N)-L
   float3 reflVect = 2.0 * NdotI * Nr - (-I);
   OUT.Cls = texCUBE(SpecularEnvMap, reflVect).rgb;
  }
 }
}

/*
void IlluminateEnvLight(in float3 worldNormal, in float3 Ps, out IncidentLight OUT)
{
 float3 worldEyeDir = normalize(Ps - g_eyePos.xyz); 

 float4 spec = g_envSpecularColor*g_specularFactor; 
 if (g_bHasSpecularEnvMap) 
 {
  spec *= SampleEnvironment((-worldEyeDir), worldNormal, 
    specularEnvSampler, g_specularEnvAngle);
 }

 OUT.Cls = spec.rgb;
  
 float4 diff = g_envDiffuseColor*g_diffuseFactor;
 if (g_bHasDiffuseEnvMap)
 {
  diff *= float4(SampleEnvDiffuse(worldNormal, 
    diffuseEnvSampler, g_diffuseEnvAngle).rgb,1);
 }

 OUT.Cld = diff.rgb;

 //OUT.L = light.Pos.xyz - Ps;
 OUT.L = worldNormal;
}
*/

/*************** eof ****************/

float4 g_diffuse : MaterialDiffuse	// multiLightVS, ibl
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Diffuse Color";
	string SasUiDescription = "diffuse color of surface";
	string UiCategory = "Diffuse";
	int UiIndex = 1;
> = {1.0f, 1.0f, 1.0f, 1.0f};

float4 g_specular : MaterialSpecular	// multiLightVS
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Specular Color";
	string SasUiDescription = "color of specular hilight";
	string UiCategory = "Specular";
	int UiIndex = 2;
> = {1.0f, 1.0f, 1.0f, 1.0f};

float g_shininess : MaterialPower	// multiLightVS
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Shininess";
	string SasUiDescription = "specular exponent controls size of highlight";
	string UiCategory = "Specular";
	float SasUiMin = 15.0;
	float SasUiMax = 1000.0;
	int UiIndex = 3;
> = 20.0f;

float3 MyDiffuse(float3 normal, float3 lightDir, float3 lDiffColor, float3 sDiffColor)
{
	float cosine = saturate(dot(normal,lightDir));
	return cosine * (lDiffColor * sDiffColor);
}

float3 MySpecular(float3 normal, float3 lightDir, float3 eyeDir, 
	float3 lSpecColor, float3 sSpecColor, float sSpecPower)
{
	// eyeDir is passed as dir FROM shade point TO eye
	// lightDir is passed as dir FROM shade point TO light

	float specComp = saturate(dot(normal,normalize(lightDir + eyeDir)));
    specComp = (dot(normal, lightDir)>0) * (pow(specComp, max(0.001, sSpecPower)));
	return specComp * (lSpecColor * sSpecColor);
}


/*****************************************************************************
/* SurfaceShader()
/*
/* ShaderCompiler replaces "SurfaceShader()" with this function stub:
/****************************************************************************/
float4 SurfaceShader(float3 Ng, float3 N, float3 L, float3 I, float3 E, float3 P, float3 Cld, float3 Cls, float4 Csd, float4 Css, float Os, float g_bumpMapScale)
//float4 SurfaceShader(float3 Ng, float3 N, float3 L, float3 I, float3 E, float3 P, float3 Cld, float3 Cls, float4 Csd, float4 Css, float Os, float g_bumpMapScale)
{

	float3 diff = MyDiffuse( N, L, Cld, g_diffuse );
	float3 spec = MySpecular( N, L, -I, Cls, g_specular , g_shininess );

	// Figure out color however you like
	return float4( diff + spec, 1 );
}
/*****************************************************************************
/* FOOTER
/****************************************************************************/
/* data from application vertex buffer */
struct appdata {
 float3 Position : POSITION;
 float3 Normal : NORMAL;
 float4 UV  : TEXCOORD0;
 float3 T  : TANGENT;
 float3 B  : BINORMAL;
};

/* data passed from vertex shader to pixel shader */
struct vertexOutput {
 float4 HPosition : POSITION;
 float4 TexCoord0 : TEXCOORD0;
 float4 UV   : TEXCOORD1;
 float4 diffCol  : COLOR0;
 float4 specCol  : COLOR1;
 float3 WorldEyeDir : TEXCOORD2;
 float3 WorldTanMatrixX : TEXCOORD3;
 float3 WorldTanMatrixY : TEXCOORD4;
 float3 WorldTanMatrixZ : TEXCOORD5;
};

sampler2D diffuseEnvSampler = sampler_state
{
 Texture = <diffuseEnvMap>;
 MipFilter = LINEAR;
 MinFilter = Anisotropic;
 MaxAnisotropy = 16;
 MagFilter = LINEAR;
 AddressU = WRAP; // Allow 3D hardware to address cubemap's correctly
 AddressV = CLAMP; // Allow 3D hardware to address cubemap's correctly
};
sampler2D specularEnvSampler = sampler_state
{
 Texture = <specularEnvMap>;
 MipFilter = LINEAR;
 MinFilter = Anisotropic;
 MaxAnisotropy = 16;
 MagFilter = LINEAR;
 AddressU = WRAP; // Allow 3D hardware to address cubemap's correctly
 AddressV = CLAMP; // Allow 3D hardware to address cubemap's correctly
};

/*********** support functions ******/
void IlluminateEnvLight(in float3 worldNormal, in float3 Ps, out IncidentLight OUT)
{
 float3 worldEyeDir = normalize(Ps - g_eyePos.xyz); 

 float4 spec = g_envSpecularColor*g_specularFactor; 
 if (g_bHasSpecularEnvMap) 
 {
  spec *= SampleEnvironment((-worldEyeDir), worldNormal, 
    specularEnvSampler, g_specularEnvAngle);
 }

 OUT.Cls = spec.rgb;
  
 float4 diff = g_envDiffuseColor*g_diffuseFactor;
 if (g_bHasDiffuseEnvMap)
 {
  diff *= float4(SampleEnvDiffuse(worldNormal, 
    diffuseEnvSampler, g_diffuseEnvAngle).rgb,1);
 }

 OUT.Cld = diff.rgb;

 //OUT.L = light.Pos.xyz - Ps;
 OUT.L = worldNormal;
}

/*********** vertex shader ******/
DOFvertexOutput DOFPrep_VS(appdata IN,
 uniform float4x4 WorldViewProj,
 uniform float4x4 WorldView)
{
 DOFvertexOutput OUT; 
 // output position in proj space
 float4 Po = float4(IN.Position, 1.0f);
 OUT.HPosition = mul(WorldViewProj, Po);
 OUT.ViewSpacePos = mul(WorldView, Po);
 return OUT;
}
DOFvertexOutput DOFPrep_VS_Tess(VS_INPUT_TESS IN)
{
 appdata Vtx;
 Tessellate(IN, Vtx);
 return DOFPrep_VS(Vtx,g_wvp,g_wv);
}
DOFvertexOutput DOFPrep_VS_Skin(VS_INPUT_SKINNING IN)
{
 appdata Vtx;
 Skin(IN, Vtx);
 return DOFPrep_VS(Vtx,g_wvp,g_wv);
}
DOFvertexOutput DOFPrep_VS_Default(appdata Vtx)
{
 return DOFPrep_VS(Vtx,g_wvp,g_wv);
}

TANGENT_VERTEX_OUTPUT singleLightVS(appdata IN,
 uniform float4x4 WorldViewProj,
 uniform float4x4 WorldIT,
 uniform float4x4 World,
 uniform float4x4 ViewIT,
 uniform float BumpMapScale,
 uniform float GlowSize
) {
 TANGENT_VERTEX_OUTPUT OUT;

 OUT.V.TexCoord0 = mul(g_uvTransform, IN.UV);
 
 float3 newPos = IN.Position;

 // output position in proj space
 float4 Po = float4(newPos + IN.Normal*GlowSize, 1.0);
 OUT.HPosition = TransformVertex(Po, IN.UV, WorldViewProj);
 
 // transform position to world space and get vector to light
 float3 Pw = mul(World, Po).xyz;
 OUT.V.WorldPos = Pw;

 OUT.V.WorldTan = TransformTangents( World, IN.T, IN.B, IN.Normal );

 // decal and bump texture coords
 OUT.V.UV = IN.UV;

 OUT.ScreenPos = float3(0,0,0);//not used

 return OUT;
}

TANGENT_VERTEX_OUTPUT singleLightVS_Tess(VS_INPUT_TESS IN) 
{
 appdata Vtx;
 Tessellate(IN, Vtx);
 return singleLightVS( Vtx, g_wvp,g_worldIT,
     g_world, g_viewIT, g_bumpMapScale, g_glowSize );
}
TANGENT_VERTEX_OUTPUT singleLightVS_Skin(VS_INPUT_SKINNING IN) 
{
 appdata Vtx;
 Skin(IN, Vtx);
 return singleLightVS( Vtx, g_wvp,g_worldIT,
     g_world, g_viewIT, g_bumpMapScale, g_glowSize );
}
TANGENT_VERTEX_OUTPUT singleLightVS_Default(appdata Vtx) 
{
 return singleLightVS( Vtx, g_wvp,g_worldIT,
     g_world, g_viewIT, g_bumpMapScale, g_glowSize );
}

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
     uniform sampler2D NormalMap,
     uniform LightInfo i_Light,
     float vFace : VFACE)
{
 pixelOutput OUT; 

 IncidentLight light;
 IlluminatePointLight(IN.V.WorldPos, i_Light, light);

 float3 Ng = float3(IN.V.WorldTan.X.z,IN.V.WorldTan.Y.z,IN.V.WorldTan.Z.z);
 float3 N = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
 float3 L = normalize(light.L);
 float3 I = normalize(IN.V.WorldPos - g_eyePos.xyz);
 float3 E = g_eyePos.xyz;
 float3 P = IN.V.WorldPos;
 float3 Cld = light.Cld;
 float3 Cls = light.Cls;
 
 OUT.col.rgba = SurfaceShader( Ng, N, L, I, E, P, Cld, Cls, Csd, Css, Os, g_bumpMapScale ); 
 
 return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
        uniform sampler2D NormalMap,
        uniform LightInfo i_Light,
        float vFace : VFACE )
{
 TangentDisplace( IN.V );
 return singleLightPS( IN, NormalMap, i_Light, vFace );
}

pixelOutput singleLightPS_Skin( TANGENT_VERTEX_OUTPUT IN,
        uniform sampler2D NormalMap,
        uniform LightInfo i_Light,
        float vFace : VFACE )
{
 return singleLightPS( IN, NormalMap, i_Light, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,         
        uniform sampler2D NormalMap,       
        uniform LightInfo i_Light,       
         float vFace : VFACE )
{
 return singleLightPS( IN, NormalMap, i_Light, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
  uniform sampler2D NormalMap,
  uniform LightInfo i_Light,
  uniform ProjLightInfo i_ProjLight,
  uniform int nBlockerSamples, 
  uniform int nShadowSamples,
  float vFace : VFACE)
{
 pixelOutput OUT; 

 IncidentLight light;
 IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, projSampler, shadowMapSampler, nBlockerSamples, nShadowSamples, light);
 
 float3 Ng = float3(IN.V.WorldTan.X.z,IN.V.WorldTan.Y.z,IN.V.WorldTan.Z.z);
 float3 N = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
 float3 L = normalize(light.L);
 float3 I = normalize(IN.V.WorldPos - g_eyePos.xyz);
 float3 E = g_eyePos.xyz;
 float3 P = IN.V.WorldPos;
 float3 Cld = light.Cld;
 float3 Cls = light.Cls;
 
 OUT.col.rgba = SurfaceShader( Ng, N, L, I, E, P, Cld, Cls, Csd, Css, Os, g_bumpMapScale ); 
 
 return OUT;
}

pixelOutput projLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
       uniform sampler2D NormalMap,
       uniform LightInfo i_Light,
       uniform ProjLightInfo i_ProjLight,
       uniform int nBlockerSamples, uniform int nShadowSamples, 
        float vFace : VFACE )
{
 TangentDisplace( IN.V );
 return projLightPS( IN, NormalMap, i_Light, i_ProjLight, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Skin( TANGENT_VERTEX_OUTPUT IN,
       uniform sampler2D NormalMap,
       uniform LightInfo i_Light,
       uniform ProjLightInfo i_ProjLight,
       uniform int nBlockerSamples, uniform int nShadowSamples, 
        float vFace : VFACE )
{
 return projLightPS( IN, NormalMap, i_Light, i_ProjLight, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
        uniform sampler2D NormalMap,
        uniform LightInfo i_Light,
        uniform ProjLightInfo i_ProjLight,
        uniform int nBlockerSamples, uniform int nShadowSamples, 
        float vFace : VFACE )
{
 return projLightPS( IN, NormalMap, i_Light, i_ProjLight, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
  uniform sampler2D NormalMap,
  uniform LightInfo i_Light,
  uniform ProjLightInfo i_ProjLight,
  float vFace : VFACE
) {
 pixelOutput OUT; 
 OUT.col = float4(0,0,0,0);
 
 float4 mask = g_bHasMask ? tex2D(glowSampler, IN.V.TexCoord0) : float4(1,1,1,1);
 if (g_bConstGlow)
 {
  OUT.col = mask;
  OUT.col.a = OUT.col.r;
 }
 else //if (mask.r+mask.g+mask.b > 0)
 {
  IncidentLight light;
  if (g_bProjLt)
   IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, projSampler, shadowMapSampler, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, light);
  else
   IlluminatePointLight(IN.V.WorldPos, i_Light, light);
    
  float3 Ng = float3(IN.V.WorldTan.X.z,IN.V.WorldTan.Y.z,IN.V.WorldTan.Z.z);
  float3 N = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
  float3 L = normalize(light.L);
  float3 I = normalize(IN.V.WorldPos - g_eyePos.xyz);
  float3 E = g_eyePos.xyz;
  float3 P = IN.V.WorldPos;
  float3 Cld = float3(0,0,0);
  float3 Cls = light.Cls;
  
  OUT.col.rgba = SurfaceShader( Ng, N, L, I, E, P, Cld, Cls, Csd, Css, Os, g_bumpMapScale ); 
  
  OUT.col *= mask;
  
 }
 OUT.col.a = 1; 

 return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
      uniform sampler2D NormalMap,
      uniform LightInfo i_Light,
      uniform ProjLightInfo i_ProjLight,
      float vFace : VFACE )
{
 TangentDisplace( IN.V );
 return glowPS( IN, NormalMap, i_Light, i_ProjLight, vFace );
}

pixelOutput glowPS_Skin( TANGENT_VERTEX_OUTPUT IN,
      uniform sampler2D NormalMap,
      uniform LightInfo i_Light,
      uniform ProjLightInfo i_ProjLight,
      float vFace : VFACE ) 
{
 return glowPS( IN, NormalMap, i_Light, i_ProjLight, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
       uniform sampler2D NormalMap, 
       uniform LightInfo i_Light,
       uniform ProjLightInfo i_ProjLight,
       float vFace : VFACE )
{
 return glowPS( IN, NormalMap, i_Light, i_ProjLight, vFace );
}


pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN,
   uniform sampler2D NormalMap, float vFace : VFACE) 
{
 pixelOutput OUT; 
 IncidentLight light; 
 
 float3 N = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
 
 IlluminateEnvLight( N , IN.V.WorldPos, light);
  
 float3 Ng = float3(IN.V.WorldTan.X.z,IN.V.WorldTan.Y.z,IN.V.WorldTan.Z.z); 
 float3 L = normalize(light.L);
 float3 I = normalize(IN.V.WorldPos - g_eyePos.xyz);
 float3 E = g_eyePos.xyz;
 float3 P = IN.V.WorldPos;
 float3 Cld = light.Cld;
 float3 Cls = light.Cls;
 
 OUT.col.rgba = SurfaceShader( Ng, N, L, I, E, P, Cld, Cls, Csd, Css, Os, g_bumpMapScale );  

 return OUT;
}

pixelOutput iblPS_Tess( TANGENT_VERTEX_OUTPUT IN, uniform sampler2D NormalMap, float vFace : VFACE )
{
 TangentDisplace( IN.V );
 return iblPS( IN, NormalMap, vFace );
}

pixelOutput iblPS_Skin( TANGENT_VERTEX_OUTPUT IN, uniform sampler2D NormalMap, float vFace : VFACE )
{
 return iblPS( IN, NormalMap, vFace );
}

pixelOutput iblPS_Default( TANGENT_VERTEX_OUTPUT IN, uniform sampler2D NormalMap, float vFace : VFACE )
{
 return iblPS( IN, NormalMap, vFace );
}
/*************/

technique Default
{
#define PASS_DEFAULT(PassName) \
 pass P##PassName   \
 {      \
  VertexShader = compile vs_3_0 singleLightVS_##PassName();\
  PixelShader = compile ps_3_0 singleLightPS_##PassName(normalSampler, g_lightInfo);\
 }
PASS_DEFAULT(Default)
PASS_DEFAULT(Tess)
//PASS_DEFAULT(Skin)
}

technique SingleLight
{
#define PASS_SINGLELIGHT(PassName) \
 pass P##PassName   \
 {      \
  VertexShader = compile vs_3_0 singleLightVS_##PassName();\
  PixelShader = compile ps_3_0 singleLightPS_##PassName(normalSampler, g_lightInfo);\
 }
PASS_SINGLELIGHT(Default)
PASS_SINGLELIGHT(Tess)
//PASS_SINGLELIGHT(Skin)
}

technique ProjectedLight
{
#define PASS_PROJECTEDLIGHT(PassName) \
 pass P##PassName   \
 {      \
  VertexShader = compile vs_3_0 singleLightVS_##PassName();\
  PixelShader = compile ps_3_0 projLightPS_##PassName(normalSampler, g_lightInfo, g_projLight, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW);\
 }
PASS_PROJECTEDLIGHT(Default)
PASS_PROJECTEDLIGHT(Tess)
//PASS_PROJECTEDLIGHT(Skin)
}
technique ProjectedLightSuperSample
{
#define PASS_PROJECTEDLIGHTSS(PassName) \
 pass P##PassName   \
 {      \
  VertexShader = compile vs_3_0 singleLightVS_##PassName();\
  PixelShader = compile ps_3_0 projLightPS_##PassName(normalSampler, g_lightInfo, g_projLight, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED);\
 }
PASS_PROJECTEDLIGHTSS(Default)
PASS_PROJECTEDLIGHTSS(Tess)
//PASS_PROJECTEDLIGHTSS(Skin)
}
technique ProjectedLightSuperSample2
{
#define PASS_PROJECTEDLIGHTSS2(PassName) \
 pass P##PassName   \
 {      \
  VertexShader = compile vs_3_0 singleLightVS_##PassName();\
  PixelShader = compile ps_3_0 projLightPS_##PassName( normalSampler, g_lightInfo, g_projLight, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH);\
 }
PASS_PROJECTEDLIGHTSS2(Default)
PASS_PROJECTEDLIGHTSS2(Tess)
//PASS_PROJECTEDLIGHTSS2(Skin)
}
technique ProjectedLightSuperSample3
{
#define PASS_PROJECTEDLIGHTSS3(PassName) \
 pass P##PassName   \
 {      \
  VertexShader = compile vs_3_0 singleLightVS_##PassName();\
  PixelShader = compile ps_3_0 projLightPS_##PassName( normalSampler, g_lightInfo, g_projLight, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH);\
 }
PASS_PROJECTEDLIGHTSS3(Default)
PASS_PROJECTEDLIGHTSS3(Tess)
//PASS_PROJECTEDLIGHTSS3(Skin)
}

technique Glow
{
#define PASS_GLOW(PassName) \
 pass P##PassName   \
 {      \
  VertexShader = compile vs_3_0 singleLightVS_##PassName();\
  PixelShader = compile ps_3_0 glowPS_##PassName(normalSampler, g_lightInfo, g_projLight);\
 }
PASS_GLOW(Default)
PASS_GLOW(Tess)
//PASS_GLOW(Skin)
}
technique DOFPrep
{
#define PASS_DOFPREP(PassName) \
 pass P##PassName   \
 {      \
  VertexShader = compile vs_3_0 DOFPrep_VS_##PassName();\
  PixelShader = compile ps_3_0 DOFPrep_PS();\
 }
PASS_DOFPREP(Default)
PASS_DOFPREP(Tess)
//PASS_DOFPREP(Skin)
}
technique Matte
{
#define PASS_MATTE(PassName) \
 pass P##PassName   \
 {      \
  VertexShader = compile vs_3_0 singleLightVS_##PassName();\
  PixelShader = compile ps_3_0 simpleMattePS(); \
 }
PASS_MATTE(Default)
PASS_MATTE(Tess)
//PASS_MATTE(Skin)
}
technique Environment
{
#define PASS_ENVIRONMENT(PassName) \
 pass P##PassName   \
 {      \
  VertexShader = compile vs_3_0 singleLightVS_##PassName();\
  PixelShader = compile ps_3_0 iblPS_##PassName(normalSampler);\
 }
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
//PASS_ENVIRONMENT(Skin)
}
/***************************** eof ***/
