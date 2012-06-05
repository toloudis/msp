/*****************************************************************************
**  SpecularFresnel.fx
**
**      Hair shader
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "Daniel Toloudis";
  string SasEffectAuthoringSoftware = "Fusion Cinema 3D";
  string SasEffectCategory			= "/material";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "Specular Fresnel";
  string SasEffectHelp				= "This is a hair-like shader with 2 anisotropic specular highlights.";    
  string SasEffectRevision			= "1";  
>;

/*********** support data and functions ******/

#define FC3D 1
#include "Support.h"
#include "Tessellate.h"
#include "Lighting.h"

// diffuse color
float4 g_hairBaseColor : MaterialDiffuse
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Diffuse Color";
	string SasUiDescription = "hair tint color";
	string UiCategory = "Diffuse";
	int UiIndex = 1;
>
= {1.0f, 1.0f, 1.0f, 1.0f};

bool hasBaseMap = false;
Texture2D tBase			: DiffuseTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Diffuse Map";
	string SasUiDescription = "diffuse color";
	string UiCategory = "Diffuse";
	string ExistVar = "hasBaseMap";
	int UiIndex = 2;
>
;

float4 g_specularColor0
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Specular Color 0";
	string SasUiDescription = "specular hilight color";
	string UiCategory = "Specular";
	int UiIndex = 3;
>
= {1.0f, 0.0f, 0.0f, 1.0f};

float4 g_specularColor1
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Specular Color 1";
	string SasUiDescription = "specular hilight color";
	string UiCategory = "Specular";
	int UiIndex = 4;
>
= {0.0f, 1.0f, 0.0f, 1.0f};

// 2 specular exponents (0..200)
float g_specularExp0
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Exponent 0";
	string SasUiDescription = "tightness of specular hilight";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 200.0;
	int UiIndex = 5;
>
= 100;

float g_specularExp1
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Exponent 1";
	string SasUiDescription = "tightness of specular hilight";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 200.0;
	int UiIndex = 6;
>
= 100;

// 2 different specular shift values (-1..1)
float g_specularShift0
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Shift 0";
	string SasUiDescription = "shift for specular hilight";
	string UiCategory = "Specular";
	float SasUiMin = -1.0;
	float SasUiMax = 1.0;
	int UiIndex = 7;
>
= -.15f;

float g_specularShift1
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Shift 1";
	string SasUiDescription = "shift for specular hilight";
	string UiCategory = "Specular";
	float SasUiMin = -1.0;
	float SasUiMax = 1.0;
	int UiIndex = 8;
>
= .15f;

bool hasSpecularMask = false;
Texture2D tSpecularMask
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Mask Map";
	string SasUiDescription = "specular mask map";
	string UiCategory = "Specular";
	string ExistVar = "hasSpecularMask";
	int UiIndex = 9;
>
;

bool hasSpecularShift = false;
Texture2D tSpecularShift
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Shift Map";
	string SasUiDescription = "specular shift map";
	string UiCategory = "Specular";
	string ExistVar = "hasSpecularShift";
	int UiIndex = 10;
>
;

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

SamplerState AnisoWrapSampler
{
	FILTER = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerState AnisoClampSampler
{
	FILTER = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = CLAMP;
};


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

technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS_##PassName(tBase, normalMap, tSpecularShift, \
					g_lightInfo, true);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_DEFAULT(Default)
PASS_DEFAULT(Tess)
}

technique11 SingleLight
{
#define PASS_SINGLELIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS_##PassName(tBase, normalMap, tSpecularShift, \
					g_lightInfo, false);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_SINGLELIGHT(Default)
PASS_SINGLELIGHT(Tess)
}

technique11 ProjectedLight
{
#define PASS_PROJECTEDLIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS_##PassName(tBase, normalMap, tSpecularShift, \
					g_lightInfo, g_projLight,\
						projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHT(Default)
PASS_PROJECTEDLIGHT(Tess)
}
technique11 ProjectedLightSuperSample
{
#define PASS_PROJECTEDLIGHTSS(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS_##PassName(tBase, normalMap, tSpecularShift, \
					g_lightInfo, g_projLight,\
						projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSS(Default)
PASS_PROJECTEDLIGHTSS(Tess)
}
technique11 ProjectedLightSuperSample2
{
#define PASS_PROJECTEDLIGHTSS2(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS_##PassName(tBase, normalMap, tSpecularShift, \
					g_lightInfo, g_projLight,\
						projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSS2(Default)
PASS_PROJECTEDLIGHTSS2(Tess)
}
technique11 ProjectedLightSuperSample3
{
#define PASS_PROJECTEDLIGHTSS3(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS_##PassName(tBase, normalMap, tSpecularShift, \
					g_lightInfo, g_projLight,\
						projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSS3(Default)
PASS_PROJECTEDLIGHTSS3(Tess)
}

technique11 Glow
{
#define PASS_GLOW(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 glowPS_##PassName(normalMap, tSpecularShift,\
					g_lightInfo, g_projLight, \
					projLightMap, projShadowMap);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_GLOW(Default)
PASS_GLOW(Tess)
}
technique11 DOFPrep
{
#define PASS_DOFPREP(PassName)	\
	pass P##PassName			\
	{						\
		SetVertexShader(CompileShader(vs_5_0, singleLightVS_##PassName()));\
		SetPixelShader(CompileShader(ps_5_0, DOFPrepTrans_PS( false, NULL, 1 )));\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_DOFPREP(Default)
PASS_DOFPREP(Tess)
}
technique11 Matte
{
#define PASS_MATTE(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 simpleMattePS();\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_MATTE(Default)
PASS_MATTE(Tess)
}
technique11 Environment
{
#define PASS_ENVIRONMENT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 iblPS_##PassName();\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
}
/***************************** eof ***/
