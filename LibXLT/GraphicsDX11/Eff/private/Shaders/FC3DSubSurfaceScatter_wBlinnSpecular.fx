/*****************************************************************************
**  SubSurfaceScatter_wBlinnSpecular.fx
**
Specular Map alpha is used for glossiness.

Features:
	- Diffuse
	- Soft Light blending algorithm
	- Normal map lighting model
	- Translucency kludge (based off melanin and hemoglobin tones)
	- Correct fresnel ramped specular

Known nasties:
	- Controls still quite tweaky
	- No depth ramp, it's a constant 
	(eg, no differentiation between a thin ear and a thick skull)
	
Referances & thanks:
	Ben Cloward - used your normal lighting method too :)
	Nvidia samples
	Direct X sdk
	Shader X2
	Polycount
	Sumea
	CGtalk
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "Daniel Toloudis & Rodrigo Urra";
  string SasEffectAuthoringSoftware = "Fusion Cinema 3D";
  string SasEffectCategory			= "/material";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "SubSurfaceScatter w/BlinnSpecular";
  string SasEffectHelp				= "This is a skin-like shader with subsurface scattering & blinn specular.";    
  string SasEffectRevision			= "1";  
>;

#define FC3D 1
#include "Support.h"
#include "Tessellate.h"
#include "Lighting.h"


bool hasDiffTex = false;
Texture2D diffTex : DiffuseTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Diffuse Map";
	string SasUiDescription = "diffuse skin map";
	string UiCategory = "Translucency";
	string ExistVar = "hasDiffTex";
	int UiIndex = 1;
>;

float4 g_transColIn : MaterialDiffuse
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Unscattered";
	string SasUiDescription = "unscattered color";
	string UiCategory = "Translucency";
	int UiIndex = 2;
>  = {0.87f, 0.91f, 0.96f, 1.0f};

bool hasTransMapIn = false;
Texture2D transMapIn
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Unscattered Map";
	string SasUiDescription = "unscattered map";
	string UiCategory = "Translucency";
	string ExistVar = "hasTransMapIn";
	int UiIndex = 3;
>;

float4 g_transColOut
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Melanin";
	string SasUiDescription = "melanin color";
	string UiCategory = "Translucency";
	int UiIndex = 4;
>  = {1.0f, 0.71f, 0.32f, 1.0f};

bool hasTransMapOut = false;
Texture2D transMapOut
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Melanin Map";
	string SasUiDescription = "melanin map";
	string UiCategory = "Translucency";
	string ExistVar = "hasTransMapOut";
	int UiIndex = 5;
>;

float4 g_transColBack
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Hemoglobin";
	string SasUiDescription = "hemoglobin color";
	string UiCategory = "Translucency";
	int UiIndex = 6;
>  = {0.58f, 0.2f, 0.24f, 1.0f};

bool hasTransMapBack = false;
Texture2D transMapBack
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Hemoglobin Map";
	string SasUiDescription = "hemoglobin map";
	string UiCategory = "Translucency";
	string ExistVar = "hasTransMapBack";
	int UiIndex = 7;
>;

float g_transMultiplier
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Translucent Power";
	string SasUiDescription = "translucent power";
	string UiCategory = "Translucency";
	float SasUiMin = 0.0;
	float SasUiMax = 2.0;
	float SasUiSteps = 2000.0;
	int UiIndex = 8;
> 	= 1.0;

bool hasTransTex = false;
Texture2D transTex
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Translucency Map";
	string SasUiDescription = "Translucency Map";
	string UiCategory = "Translucency";
	string ExistVar = "hasTransTex";
	int UiIndex = 9;
>;

float g_transRampOff
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Translucent Ramp Off";
	string SasUiDescription = "translucent ramp off";
	string UiCategory = "Translucency";
	float SasUiMin = 0.0;
	float SasUiMax = 2.0;
	float SasUiSteps = 1000.0;
	int UiIndex = 10;
> 	= 1.0;

float4 g_specular
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Specular Color";
	string SasUiDescription = "color of specular highlight";
	string UiCategory = "Specular";
	int UiIndex = 11;
>
= {0.45f, 0.65f, 1.0f, 1.0f};

float g_specularPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Power";
	string SasUiDescription = "multiplier for specular contribution";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 13;
>
= 0.2;

float g_IOR
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Roll Off";
	string SasUiDescription = "specular fresnel index of refraction";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 14;
> = 1.0;

bool hasIORMap = false;
Texture2D iorMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Roll Off Map";
	string SasUiDescription = "specular fresnel index of refraction texture";
	string UiCategory = "Specular";
	string ExistVar = "hasIORMap";
	int UiIndex = 15;
>
;

float g_shininess : MaterialPower 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Shininess";
	string SasUiDescription = "specular exponent controls size of highlight";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 16;
> = 0.5f;

bool hasGlossMap = false;
Texture2D glossMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Shininess Map";
	string SasUiDescription = "specular exponent multiplier";
	string UiCategory = "Specular";
	string ExistVar = "hasGlossMap";
	int UiIndex = 17;
>
;

float g_specularBias = 0;

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

//=======================Translucency lighting model=======================
float4 TransPass(	float4 DotLN,
			float2 TexUV)
{
	float4 transSample = Tex2DReplace(hasTransTex, transTex, TexUV, 0);
	
	static const float4 one = float4(1,1,1,1);	
	float4 Translucence 	= smoothstep(-g_transRampOff  * transSample,one,abs(DotLN)) 
					;//- smoothstep(one,one,DotLN); // or should it be step(one, DotLN);//???
	
	float4 transColMapIn = Tex2DCombine(hasTransMapIn, transMapIn, TexUV, g_transColIn);
	float4 transColMapOut = Tex2DCombine(hasTransMapOut, transMapOut, TexUV, g_transColOut);
	float4 transColMapBack = Tex2DCombine(hasTransMapBack, transMapBack, TexUV, g_transColBack);
	float4 Colourise		= lerp(transColMapBack, lerp(transColMapOut * g_transMultiplier,transColMapIn,(DotLN)),Translucence);
	
	return (Colourise * Translucence);
}


float3 BlinnSpecular(float3 normal, float3 lightDir, float3 eyeDir, 
	float3 lSpecColor, float3 sSpecColor, float eccentricity, float IOR,
	float specularBias)
{
	// eyeDir is passed as dir FROM shade point TO eye
	// lightDir is passed as dir FROM shade point TO light

	float3 H = normalize(lightDir + eyeDir);
	// NdotH = cos a
	float NdotH = dot(normal, H);
	float NdotE = dot(normal, eyeDir);
	float EdotH = dot(eyeDir, H);
	float NdotL = dot(normal, lightDir);
	
	float Gb = 2 * NdotH * NdotE / EdotH;
	float Gc = 2 * NdotH * NdotL / EdotH; 
	float G = min(Gb, Gc);
	G = min(1, G);
	
	// eccentricity varies from 0 to 1. 
	// D3 = [ c^2 / (1 + cos^2(a)(c^2 - 1)) ]^2
	float c2 = eccentricity*eccentricity;
	float D3 = ( c2 / ( 1 + (NdotH*NdotH*(c2-1)) ) );
	float D = D3*D3;
	
	// eta = ni/nt. assume air-material is the interface
	float F = fresnel(NdotE, 1/(IOR+0.0001)); 

	float specComp = D * G * F / NdotE;
	specComp = max(specComp, 0);
	return (specComp * g_specularPower) * (lSpecColor * sSpecColor);
}

//================================Pixel shader - Complete================================

SamplerState AnisoWrapSampler
{
	FILTER = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = WRAP;
};

float3 SkinShader(float2 TexUV,  
	float3 WN, // normalize(world space normal)
	float3 EV, //= normalize(IN.WorldEyeDir);     //(world space)
	float3 LV, //= normalize(IN.LightVector.xyz); //(world space)
	uniform float3 LightColourDiff,
	uniform float3 LightColourSpec)
{
	float4 DotLN = dot(LV,WN);

    float4 a = Tex2DReplace(hasDiffTex, diffTex, TexUV, 1);
    float4 b = TransPass(DotLN,TexUV);
    
//	float4 a 			= tex2D(DiffMap,TexUV);	
//	float4 BaseLighting	= ((1 - a) * (a*b) + a * (1 - (1 - a) * (1 - b))) * b;

	float4 BaseLighting = (a * b)*(a + b + b - 2*a*b);
	
	float3 sSpecColor = g_specular.rgb;
	float sSpecPower  = Tex2DCombine(hasGlossMap, glossMap, TexUV, g_shininess).r;	
	float ior		  = Tex2DCombine(hasIORMap, iorMap, TexUV, g_IOR-1).r + 1;
	
	float3 specularComponent = BlinnSpecular(WN, LV, EV, LightColourSpec, sSpecColor, 
											 sSpecPower, ior, g_specularBias);
	
	float3 LightingOutput = BaseLighting.rgb * LightColourDiff + 
							specularComponent.rgb * LightColourSpec.rgb;
				
	return LightingOutput;
} 

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform LightInfo i_Light,
					uniform bool i_bDefaultPass,
					bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT;

	//early alpha test
	OUT.col.a = 1;

	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);

	//fetch bump normal
	float3 worldNormal = GetNormal( IN.V, vFace );
//	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

	OUT.col.rgb = SkinShader(IN.V.TexCoord0, normalize(worldNormal),
		-worldEyeDir, lightDir, light.Cld, light.Cls);
   
	if (i_bDefaultPass)
	{
		OUT.col.rgb += envmap_approximation(g_diffuseFactor).rgb;	
	}

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							   uniform Texture2D DiffuseMap,
								uniform LightInfo i_Light,
								uniform bool i_bDefaultPass,
								bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, DiffuseMap, i_Light, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								  uniform Texture2D DiffuseMap,
								uniform LightInfo i_Light,
								uniform bool i_bDefaultPass,
								bool vFace : SV_ISFRONTFACE )
{
	return singleLightPS( IN, DiffuseMap, i_Light, i_bDefaultPass, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = 1;

	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch bump normal
	float3 worldNormal = GetNormal( IN.V, vFace );
//	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

	OUT.col.rgb = SkinShader(IN.V.TexCoord0, normalize(worldNormal),
		-worldEyeDir, lightDir, light.Cld, light.Cls);

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput projLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							 uniform Texture2D DiffuseMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return projLightPS( IN, DiffuseMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								uniform Texture2D DiffuseMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE )
{
	return projLightPS( IN, DiffuseMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}


pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
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
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = 1;

	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);

	float3 worldNormal = GetNormal( IN.V, vFace );
//	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

	float4 transColMapIn = 	Tex2DCombine(hasTransMapIn, transMapIn, IN.V.TexCoord0, g_transColIn);
	float4 diff = Tex2DCombine(hasDiffTex, diffTex, IN.V.TexCoord0, transColMapIn*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}
		
	OUT.col.rgb = g_IsolateReflection ? 0 : diff.rgb;
	OUT.col.rgb *= OUT.col.a;	//premul alpha

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

//================================TECHNIQUES================================


technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS_##PassName(diffTex, \
					g_lightInfo, true );\
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
		PixelShader = compile ps_5_0 singleLightPS_##PassName(diffTex, \
					g_lightInfo, false );\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffTex, \
					g_lightInfo, g_projLight, \
						projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW );\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffTex, \
					g_lightInfo, g_projLight, \
						projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED );\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffTex, \
					g_lightInfo, g_projLight, \
						projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH );\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffTex, \
					g_lightInfo, g_projLight, \
						projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH );\
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
		PixelShader = compile ps_5_0 glowPS_##PassName(\
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
