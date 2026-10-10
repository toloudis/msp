//////////////////////////////////////////////////////////////////////////////
// Converted from Support.h by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Source of truth for the plain-HLSL material shaders from here on; the
// original Support.h was deleted along with the Effects (.fx) shaders.
// Register layout: see Globals.hlsli.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Support.h
**
**      Support functions for .fx functions
**
**	Gigawatt Studios
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifndef _GLOBALS_
#include "Globals.hlsli"
#endif

//constants
static const float HALFPI = 1.57079633;
static const float PI = 3.14159265;
static const float TWOPI = 6.28318531;
static const float INVHALFPI = 0.636619772;//1.0f/1.57079633;
static const float INVPI = 0.318309886;//1.0f/3.14159265;
static const float INVTWOPI = 0.159154943;//1.0f/6.28318531;
static const float PI_DIV_180 = 0.0174532925;//PI/180

// The constants this file used to declare (g_FirstLight, the transforms,
// g_eyePos, g_targetRes, g_bumpMapScale, hasNormalMap, g_time_0_X,
// g_AlphaTestRef, g_bDoubleSided, g_IsolateReflection, g_bCubeMapEnabled,
// the environment and glow inputs) are members of the cbuffers in
// Globals.hlsli.

Texture2D normalMap : register(t0);			// : NormalMap

// Output pixel values
struct pixelOutput {
  float4 col : SV_TARGET;
};

// used for ambient environment image-based lighting
Texture2D diffuseEnvMap : register(t1);
// FC3D doesn't know about specular environments!
#ifndef FC3D
Texture2D specularEnvMap : register(t2);
#endif // FC3D

// maps a vector to normalized azimuth and elevation angles.
// angles are 0-1
// input vector is normalized
// +X is origin of X/Z plane for azimuthal angle
// +Y is up for elevation angle;
float2 CartesianToPolar( float3 vec )
{
	float3 nvec = normalize( vec );
	// fudge factor to keep denominator away from 0
	return float2( (atan2( nvec.z, nvec.x + 0.001 )+PI)*INVTWOPI, (acos( nvec.y ))*INVPI );
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
void getTangentToWorldSpace(uniform float4x4 objToWorld,
                                  in float3 objTangent,
								  in float3 objBinormal,
								  in float3 objNormal,
								  in float bumpMapScale,
//								  out float3x3 worldToTangentSpace,
								  out float3 tanToWorldX,
								  out float3 tanToWorldY,
								  out float3 tanToWorldZ)
{
	tanToWorldX = mul( (float3x3)objToWorld, objTangent).xyz;
	tanToWorldY = mul( (float3x3)objToWorld, objBinormal).xyz;
	tanToWorldZ = mul( (float3x3)objToWorld, objNormal).xyz;

/*
	float3 wTangent = mul(objToWorld, objTangent).xyz;
	float3 wBinormal = mul(objToWorld, objBinormal).xyz;
	float3 wNormal = mul(objToWorld, objNormal).xyz;
//	worldToTangentSpace[0] = wTangent * bumpMapScale;
//	worldToTangentSpace[1] = wBinormal * bumpMapScale;
//	worldToTangentSpace[2] = wNormal;

	// assumption: inverse = transpose
	// why does bumpmapscale work like this? 
	// if the above uses bumpmapscale, shouldn't this one use 1/bumpmapscale?
	// yet somehow this works.
	tanToWorldX = float3(wTangent.x*bumpMapScale, wBinormal.x*bumpMapScale, wNormal.x);
	tanToWorldY = float3(wTangent.y*bumpMapScale, wBinormal.y*bumpMapScale, wNormal.y);
	tanToWorldZ = float3(wTangent.z*bumpMapScale, wBinormal.z*bumpMapScale, wNormal.z);
*/
}

TANGENT_MATRIX TransformTangents( uniform float4x4 mat,
								 in float3 Tangent,
								 in float3 Binormal,
								 in float3 Normal )
{
	TANGENT_MATRIX tmat;
	tmat.X = mul( (float3x3)mat, Tangent);
	tmat.Y = mul( (float3x3)mat, Binormal);
	tmat.Z = mul( (float3x3)mat, Normal);
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
//		x' = z*sin q + x*cos q
//		y' = y
//		z' = z*cos q - x*sin q
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
//		x' = x
//		y' = y*cos q - z*sin q
//		z' = y*sin q + z*cos q
	}
}

SamplerState g_DefaultSampler : register(s0);

float4 Tex2DCombine(bool hasTex, Texture2D tex, float2 texCoord, float4 color)
{
//	return (hasTex) ? color * tex2D(samp, texCoord) : color;
	float4 outCol = color;
	if (hasTex)
		outCol *= tex.Sample(g_DefaultSampler, texCoord);
	return outCol;
}
float4 Tex2DReplace(bool hasTex, Texture2D tex, float2 texCoord, float4 color)
{
//	return (hasTex) ? tex2D(samp, texCoord) : color;
	float4 outCol = color;
	if (hasTex)
		outCol = tex.Sample(g_DefaultSampler, texCoord);
	return outCol;
}
float3 Tex2DNormal(bool hasTex, Texture2D tex, float2 texCoord)
{
//	return (hasTex) ? normalize(expand(tex2D(samp, texCoord))) : float3(0,0,1);
	float3 bumpNormal = float3(0,0,1);
	if (hasTex)
		bumpNormal = normalize(expand(tex.Sample(g_DefaultSampler, texCoord).xyz));
	return bumpNormal;
}

SamplerState g_CubeSampler : register(s1);

float4 SampleEnvironment(float3 worldEyeDir, float3 worldNormal,
						 uniform Texture2D CubeMap)
{
	// world eye dir points FROM shade point TO eye pos

	// do world space reflection
	float nDotV = dot(worldEyeDir, worldNormal);
	//R = 2*N*(L.N)-L
	float3 reflVect = 2.0 * nDotV * worldNormal - worldEyeDir;
	return CubeMap.SampleLevel(g_CubeSampler, CartesianToPolar(reflVect), 0);
}

float4 SampleEnvironment(float3 worldEyeDir, float3 worldNormal,
						 uniform Texture2D CubeMap, float angle)
{
	// do world space reflection
	float nDotV = dot(worldEyeDir, worldNormal);
	//R = 2*N*(L.N)-L
	float3 reflVect = 2.0 * nDotV * worldNormal - worldEyeDir;
	// Rotate about the Y axis using reflective vector
	float3 rotateVec = rotateAboutY(reflVect, angle);

	return CubeMap.SampleLevel(g_CubeSampler, CartesianToPolar(rotateVec), 0);
}

float4 SampleEnvironmentLOD(float3 worldEyeDir, float3 worldNormal,
						 uniform Texture2D CubeMap, float angle, float mipLOD)
{
	// do world space reflection
	float nDotV = dot(worldEyeDir, worldNormal);
	//R = 2*N*(L.N)-L
	float3 reflVect = 2.0 * nDotV * worldNormal - worldEyeDir;
	// Rotate about the Y axis using reflective vector
	float3 rotateVec = rotateAboutY(reflVect, angle);

	return CubeMap.SampleLevel(g_CubeSampler, CartesianToPolar(rotateVec), mipLOD);
}

float4 SampleEnvDiffuse(float3 worldNormal,
						 uniform Texture2D CubeMap)
{
	return CubeMap.SampleLevel(g_CubeSampler, CartesianToPolar(worldNormal), 0);
}
float4 SampleEnvDiffuse(float3 worldNormal,
						 uniform Texture2D CubeMap, float angle)
{
	worldNormal = rotateAboutY(worldNormal, angle);
	return CubeMap.SampleLevel(g_CubeSampler, CartesianToPolar(worldNormal), 0);
}
float4 SampleEnvDiffuseLOD(float3 worldNormal,
						 uniform Texture2D CubeMap, float angle, float mipLod)
{
	worldNormal = rotateAboutY(worldNormal, angle);
	return CubeMap.SampleLevel(g_CubeSampler, CartesianToPolar(worldNormal), mipLod);
}

// Return a color that approximates the diffuse contribution
// of the environment map. Used in non-shadow passes.
float4 envmap_approximation(float envMapDffuseFactor)
{
	return float4(0.2 * g_envDiffuseColor.rgb * envMapDffuseFactor, 0);
//	float scaled_factor = 0.8 * envMapDffuseFactor;
//	return float4(scaled_factor, scaled_factor, scaled_factor, 0);
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
    	//	specComp = (pow(specComp, sSpecPower));
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


// g_fDofBlurCutoff and g_vDofParams are in FrameParams (Globals.hlsli).

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

/*********** vertex shader ******/
DOFvertexOutput DOFPrepVS_Default( STANDARD_VERTEX IN )
{
	DOFvertexOutput OUT;
	// output position in proj space
	float4 Po = float4(IN.Position, 1.0f);
	OUT.HPosition = mul( g_wvp, Po);
	OUT.ViewSpacePos = mul( g_wv, Po);
	OUT.TexCoord0 = mul(g_uvTransform, float4(IN.UV,0,1)).xy;
	return OUT;
}

pixelOutput DOFPrep_PS(DOFvertexOutput IN) 
{
	pixelOutput OUT;

	float bl = ComputeDepthBlur(IN.ViewSpacePos.z); 
	//    float bl = (IN.ViewSpacePos.z); 
	OUT.col = float4(bl,bl,bl,bl);
	return OUT;
}

pixelOutput DOFPrepTrans_PS(TANGENT_VERTEX_OUTPUT IN, uniform bool bHasTMap, uniform Texture2D i_TMap, uniform float i_Trans ) 
{
    pixelOutput OUT;

	//early alpha test
//	float Alpha = Tex2DCombine(bHasTMap, i_TMap, IN.TexCoord0, i_Trans).r;
//	if( g_AlphaTestRef >= Alpha ) discard;

    float bl = ComputeDepthBlur(length(IN.V.WorldPos.xyz - g_eyePos.xyz));//IN.ViewSpacePos.z); 
    OUT.col = float4(bl,bl,bl,bl);
    return OUT;
}

pixelOutput simpleMattePS()
{
    pixelOutput OUT; 
    OUT.col = float4(1,1,1,1);
    return OUT;
}

//// convert vertex uv coordinate to clip space position for texture baking.
//float4 BakeVertex(in float2 i_UV, in float4x4 wvp)
//{
//	// this step assures that the baked texture captures 
//	// the entire texture space of a mesh that has pre-tiled uvs.
//	float2 untiledUV = saturate(i_UV * g_bakeTransform.xy + g_bakeTransform.zw);
//	untiledUV.y = 1.0f - untiledUV.y;
//
//	float4 o_hPos;
//
////	o_hPos.xy = untiledUV*2 - float2(1,1);
////	o_hPos.y = -o_hPos.y;
////	o_hPos.zw = i_UV.zw;
//
//	untiledUV -= 0.5;
//	untiledUV *= 2.0f;
//	
//	o_hPos = float4(untiledUV,0.5,1);
//	//o_hPos = mul(wvp, float4(untiledUV,0.5,1));
//
//
//	return o_hPos;
//}
//
//// decide how to best get the vertex to clip space, and then do it!
//float4 TransformVertex(in float4 i_Po, in float2 i_UV, in float4x4 wvp)
//{
//	float4 o_hPos;
//    if (g_bake)
//		o_hPos = BakeVertex(i_UV, wvp);
//    else
//		o_hPos = mul(wvp, i_Po);
//	return o_hPos;
//}


/* data from application vertex buffer */
struct vertexData {
    float3 Position	: SV_POSITION;
    float3 Normal	: NORMAL;
    float2 UV		: TEXCOORD0;
    float3 T		: TANGENT;
    float3 B		: BINORMAL;
};
struct shadingData {
    float4 HPosition	: SV_POSITION;
    float2 TexCoord0	: TEXCOORD0;
    float2 UV			: TEXCOORD1;
    float3 WorldPos		: TEXCOORD2;
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
	getTangentToWorldSpace( World,IN.T,IN.B,IN.Normal,BumpMapScale, 
		OUT.WorldTanMatrixX,OUT.WorldTanMatrixY,OUT.WorldTanMatrixZ);
	
	// decal and bump texture coords
    OUT.TexCoord0 = mul(g_uvTransform, float4(IN.UV,0,0)).xy;
    OUT.UV = IN.UV;
    
    return OUT;
}

// these filter funcs work for floats, but maybe not for vector quantities.
#define MINFILTERWIDTH 0.000001
float filterwidth(float x) { return max(abs(ddx(x)) + abs(ddy(x)), MINFILTERWIDTH); }
float filterarea(float x) { return (abs(ddx(x)) * abs(ddy(x))); }
float filteredstep(float edge, float x, float w) { return clamp(((x)+(w)/2-(edge))/(w), 0, 1); }


/***************************** eof ***/
