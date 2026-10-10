//////////////////////////////////////////////////////////////////////////////
// Converted from FC3DSpecularFresnel.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// FC3DSpecularFresnel.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  SpecularFresnel.fx
**
**      Hair shader
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
// The SasGlobal effect description (gp : SasGlobal) and the SAS UI
// annotations of the material parameters are in FC3DSpecularFresnel.effect.json.
// Register layout shared by all materials: see Globals.hlsli.

/*********** support data and functions ******/

#define FC3D 1
#include "Support.hlsli"
#include "Tessellate.hlsli"
#include "Lighting.hlsli"

// b4: this material's own parameters (defaults are in FC3DSpecularFresnel.effect.json)
cbuffer MaterialParams : register(b4)
{
	// diffuse color
	float4 g_hairBaseColor;			// : MaterialDiffuse, default (1,1,1,1)
	bool hasBaseMap;				// default false
	float4 g_specularColor0;		// default (1,0,0,1)
	float4 g_specularColor1;		// default (0,1,0,1)
	// 2 specular exponents (0..200)
	float g_specularExp0;			// default 100
	float g_specularExp1;			// default 100
	// 2 different specular shift values (-1..1)
	float g_specularShift0;			// default -0.15
	float g_specularShift1;			// default 0.15
	bool hasSpecularMask;			// default false
	bool hasSpecularShift;			// default false
};

Texture2D tBase : register(t4);					// : DiffuseTexture
Texture2D tSpecularMask : register(t5);
Texture2D tSpecularShift : register(t6);

SamplerState AnisoWrapSampler : register(s10);
SamplerState AnisoClampSampler : register(s11);

/*********** vertex shader ******/

TANGENT_VERTEX TangentVS( STANDARD_VERTEX In )
{
    TANGENT_VERTEX OUT;

	// decal and bump texture coords
    OUT.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;
    OUT.UV = In.UV;
    
	float3 NewPos = In.Position;
    
    // output position in proj space
    float4 Po = float4(NewPos, 1.0);
    
    // transform position to world space and get vector to light
    float3 Pw = mul(g_world, Po).xyz;
    OUT.WorldPos = Pw;

	OUT.WorldTan = TransformTangents( g_world, In.T, In.B, In.Normal );
	
    return OUT;
}

TANGENT_VERTEX_OUTPUT singleLightVS_Default( STANDARD_VERTEX Vtx, out float ClipDist : SV_ClipDistance0 ) 
{
	TANGENT_VERTEX_OUTPUT OUT;
	OUT.V = TangentVS( Vtx );

    float4 Po = float4( Vtx.Position, 1.0);
    OUT.HPosition = TransformVertex( Po, Vtx.UV, g_wvp );
	OUT.ScreenPos = float3(0,0,0);//not used

	ClipDist = ClipWorldPos( OUT.V.WorldPos );

	return OUT;
}

TANGENT_TESS_OUTPUT singleLightVS_Tess( STANDARD_VERTEX Vtx ) 
{
	TANGENT_TESS_OUTPUT OUT;
	OUT.V = TangentVS( Vtx );

	return OUT;
}

/********* pixel shader ********/


// to shift the specular highlight along the length of the hair, 
// we nudge the tangent along the direction of the normal.
// assumes T pointing from root to tip.
// positive nudge moves hilight toward root, negative - toward tip
// get shift value from texture to break up uniform look over all hair patches.
float3 ShiftTangent(float3 T, float3 N, float shift)
{
	float3 shiftedT = T + shift*N;
	return normalize(shiftedT);
}

// uses half angle vector. could use refl vector, at cost of complexity.
float StrandSpecular(float3 T, float3 H, float exponent)
{
	float dotTH = dot(T,H);
	float sinTH = saturate(sqrt(1.0 - dotTH*dotTH));
	float dirAtten = smoothstep(-1.0, 0.0, dotTH);
//	return dirAtten * pow(max(0.001,sinTH), max(0.001,exponent));
	return sinTH == 0 ? 0 : dirAtten * pow(sinTH, exponent);
}

float HairDiffuseTerm(float3 N, float3 L)
{
    return saturate(0.75 * dot(N, L) + 0.25);
}

float3 HairDiffuseContrib(float3 normal, float3 lightVec,
	float3 lightColor)
{
	// diffuse lighting: the lerp shifts the shadow boundary for a softer look
	float3 diffuse = saturate(lerp(0.25,1.0, dot(normal, lightVec)));
	diffuse *= lightColor * g_hairBaseColor.rgb;
	
	return diffuse;
}
float3 HairSpecularContrib(float3 tangent, float3 normal, float3 halfVec,
	float2 uv)
{
	// shift tangents
	float shiftTex = Tex2DReplace(hasSpecularShift, tSpecularShift, uv, 0.5).x - 0.5;
	float3 t1 = ShiftTangent(tangent, normal, g_specularShift0 + shiftTex);
	float3 t2 = ShiftTangent(tangent, normal, g_specularShift1 + shiftTex);

	// 2 hilights of different colors, specular exponents, and differently shifted tangents.

	// add 2nd specular term, modulated with noise texture
	float specMask = Tex2DReplace(hasSpecularMask, tSpecularMask, uv, 1).x; // approximate sparkles using texture
	
	// specular lighting
	float3 specular = (g_specularColor0.rgb * StrandSpecular(t1, halfVec, g_specularExp0) + 
					   g_specularColor1.rgb * StrandSpecular(t2, halfVec, g_specularExp1)) * specMask;
	
	return specular;
}

float4 HairLighting(float3 tangent, float3 normal, float3 lightVec,
	float3 halfVec, float2 uv, float ambOcc, 
	float3 lightDiffColor, float3 lightSpecColor)
{
	float3 diffuse = HairDiffuseContrib(normal, lightVec, lightDiffColor);

	float3 specular = HairSpecularContrib(tangent, normal, halfVec, uv);

    // specular attenuation for hair facing away from light
    float specularAttenuation = saturate(1.75 * dot(normal, lightVec) + 0.5);
	
	float3 base = Tex2DReplace(hasBaseMap, tBase, uv, 1).rgb;
	 	
	// final color assembly
	float4 o;

    o.rgb = diffuse * base;
    // enable this for slightly subtractive specular 
//    base = 1.5 * base - 0.5;

    o.rgb += specular * base * specularAttenuation * lightSpecColor;

//	o.rgb *= ambOcc; // modulate color by ambient occlusion term (self shadowing?)

	o.a = 1.0;
	return o;
}

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform Texture2D NormalMap,
					uniform Texture2D SpecularMap,
					uniform LightInfo i_Light,
		  			uniform bool i_bDefaultPass,
		  			bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 
    
	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch bump normal
	float3 worldNormal = GetNormal( IN.V, vFace );
//	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
    	
	// the (0,1,0) direction in tangent space:
	float3 worldTan = float3(IN.V.WorldTan.X.y, IN.V.WorldTan.Y.y, IN.V.WorldTan.Z.y);
	
	OUT.col = HairLighting(worldTan, worldNormal, lightDir,
		normalize(lightDir + -worldEyeDir), IN.V.TexCoord0, 1.0,
		light.Cld, light.Cls);
		
	if (i_bDefaultPass)
	{
		OUT.col.rgb += envmap_approximation(g_diffuseFactor).rgb;	
	}
		
	OUT.col.rgb *= OUT.col.a;	//premul alpha

	//alpha test
	if( g_AlphaTestRef >= OUT.col.a ) discard;

    return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							uniform Texture2D DiffuseMap,
							uniform Texture2D NormalMap,
							uniform Texture2D SpecularMap,
							uniform LightInfo i_Light,
				  			uniform bool i_bDefaultPass,
							bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, DiffuseMap, NormalMap, SpecularMap, i_Light, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
							uniform Texture2D DiffuseMap,
							uniform Texture2D NormalMap,
							uniform Texture2D SpecularMap,
							uniform LightInfo i_Light,
				  			uniform bool i_bDefaultPass,
							bool vFace : SV_ISFRONTFACE )
{
	return singleLightPS( IN, DiffuseMap, NormalMap, SpecularMap, i_Light, i_bDefaultPass, vFace );
}


pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 
    
	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch bump normal
	float3 worldNormal = GetNormal( IN.V, vFace );
//	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
    	
	// the (0,1,0) direction in tangent space:
	float3 worldTan = float3(IN.V.WorldTan.X.y, IN.V.WorldTan.Y.y, IN.V.WorldTan.Z.y);
	
	OUT.col = HairLighting(worldTan, worldNormal, lightDir,
		normalize(lightDir + -worldEyeDir), IN.V.TexCoord0, 1.0,
		light.Cld, light.Cls);

	OUT.col.rgb *= OUT.col.a;	//premul alpha

	//alpha test
	if( g_AlphaTestRef >= OUT.col.a ) discard;

    return OUT;
}

pixelOutput projLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							 uniform Texture2D DiffuseMap,
							uniform Texture2D NormalMap,
							uniform Texture2D SpecularMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples, 
							 bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return projLightPS( IN, DiffuseMap, NormalMap, SpecularMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								uniform Texture2D DiffuseMap,
							uniform Texture2D NormalMap,
							uniform Texture2D SpecularMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples, 
								bool vFace : SV_ISFRONTFACE )
{
	return projLightPS( IN, DiffuseMap, NormalMap, SpecularMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap, bool vFace : SV_ISFRONTFACE
) {
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,0);
    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D NormalMap,
						uniform Texture2D SpecularMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap, 
						bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, NormalMap, SpecularMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
						   uniform Texture2D NormalMap,
							uniform Texture2D SpecularMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap, 
							bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, NormalMap, SpecularMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 
    
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);

	float3 worldNormal = GetNormal( IN.V, vFace );
//	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

	float4 diff = Tex2DCombine(hasBaseMap, tBase, IN.V.TexCoord0, g_hairBaseColor*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}
		
	OUT.col = float4(
		g_IsolateReflection ? 0 : diff.rgb,
		
		1);

	OUT.col.rgb *= OUT.col.a;	//premul alpha

	//alpha test
	if( g_AlphaTestRef >= OUT.col.a ) discard;

    return OUT;
}

pixelOutput iblPS_Tess( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return iblPS( IN, vFace );
}

pixelOutput iblPS_Default( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	return iblPS( IN, vFace );
}

/*************/

/*
sampler[0] = (tBaseMap);
sampler[1] = (tNormalMapMap);
sampler[2] = (tAlphaMap);
sampler[3] = (tSpecularShiftMap);
sampler[4] = (tSpecularMaskMap);
sampler[5] = (projMap);
sampler[6] = (shadowMapMap);
sampler[7] = (glowMap);
sampler[8] = (diffuseEnvMap);
sampler[9] = (specularEnvMap);
*/


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput Default_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Default(IN, tBase, normalMap, tSpecularShift, g_lightInfo, true, vFace);
}

pixelOutput Default_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, tBase, normalMap, tSpecularShift, g_lightInfo, true, vFace);
}

pixelOutput SingleLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Default(IN, tBase, normalMap, tSpecularShift, g_lightInfo, false, vFace);
}

pixelOutput SingleLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, tBase, normalMap, tSpecularShift, g_lightInfo, false, vFace);
}

pixelOutput ProjectedLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, tBase, normalMap, tSpecularShift, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, tBase, normalMap, tSpecularShift, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLightSuperSample_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, tBase, normalMap, tSpecularShift, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, tBase, normalMap, tSpecularShift, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample2_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, tBase, normalMap, tSpecularShift, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample2_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, tBase, normalMap, tSpecularShift, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, tBase, normalMap, tSpecularShift, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, tBase, normalMap, tSpecularShift, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput Glow_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Default(IN, normalMap, tSpecularShift, g_lightInfo, g_projLight, projLightMap, projShadowMap, vFace);
}

pixelOutput Glow_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Tess(IN, normalMap, tSpecularShift, g_lightInfo, g_projLight, projLightMap, projShadowMap, vFace);
}

pixelOutput DOFPrep_PDefault_PS(TANGENT_VERTEX_OUTPUT IN)
{
    return DOFPrepTrans_PS(IN, false, normalMap, 1);	// the .fx passed NULL: the texture is unused when bHasTMap is false
}
