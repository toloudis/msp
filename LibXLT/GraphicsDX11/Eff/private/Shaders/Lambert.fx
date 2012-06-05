/*****************************************************************************
**  Lambert.fx
**
**      lambert shader
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "Daniel Toloudis";
  string SasEffectAuthoringSoftware = "MachStudio";
  string SasEffectCategory			= "/material";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "Lambert";
  string SasEffectHelp				= "This is a Lambert shader.";    
  string SasEffectRevision			= "1";  
>;

/*********** support data and functions ******/

#include "Support.h"
#include "Tessellate.h"
#include "Lighting.h"

float4 g_diffuse : MaterialDiffuse
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Diffuse Color";
	string SasUiDescription = "diffuse color of surface";
	string UiCategory = "Diffuse";
	int UiIndex = 1;
>
= {1.0f, 1.0f, 1.0f, 1.0f};

bool hasDiffuseMap = false;
Texture2D diffuseMap		: DiffuseTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Diffuse Map";
	string SasUiDescription = "diffuse color";
	string UiCategory = "Diffuse";
	string ExistVar = "hasDiffuseMap";
	int UiIndex = 2;
>
;

float4 g_emissive : MaterialEmissive
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Emissive Color";
	string SasUiDescription = "emissive color";
	string UiCategory = "Diffuse";
	int UiIndex = 3;
>
= {0.0f, 0.0f, 0.0f, 1.0f};

bool hasEmissiveMap = false;
Texture2D emissiveMap : EmissiveTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Emissive Map";
	string SasUiDescription = "emissive color";
	string UiCategory = "Diffuse";
	string ExistVar = "hasEmissiveMap";
	int UiIndex = 4;
>;

float g_emissiveIntensity 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Intensity";
	string SasUiDescription = "Emissive Intensity";
	string UiCategory = "Diffuse";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 5;
> = 1.0f;

float g_transparency : Opacity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Opacity";
	string SasUiDescription = "Opacity value, 0 transparent, 1 for opaque";
	string UiCategory = "Opacity";
	int UiIndex = 6;
>
= 1.0f;

bool hasTransparencyMap = false;
Texture2D transparencyMap	: OpacityTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Opacity Texture";
	string SasUiDescription = "Opacity texture";
	string UiCategory = "Opacity";
	string ExistVar = "hasTransparencyMap";
	int UiIndex = 7;
>
;

Texture2D glowMask		: GlowMask;

/*********** vertex shader ******/

TANGENT_VERTEX TangentVS( STANDARD_VERTEX In )
{
    TANGENT_VERTEX OUT;

	// decal and bump texture coords
    OUT.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;
    OUT.UV = In.UV;
    
	float3 NewPos = In.Position;
    
    // output position in proj space
    float4 Po = float4(NewPos + In.Normal*g_glowSize, 1.0);
    
    // transform position to world space and get vector to light
    float3 Pw = mul( g_world, Po).xyz;
    OUT.WorldPos = Pw;

	OUT.WorldTan = TransformTangents( g_world, In.T, In.B, In.Normal );
	
    return OUT;
}

TANGENT_VERTEX_OUTPUT singleLightVS_Default( STANDARD_VERTEX Vtx, out float ClipDist : SV_ClipDistance0 ) 
{
	TANGENT_VERTEX_OUTPUT OUT;
	OUT.V = TangentVS( Vtx );

    float4 Po = float4( Vtx.Position + Vtx.Normal*g_glowSize, 1.0);
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

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform Texture2D NormalMap,
					uniform LightInfo i_Light, 
					uniform bool i_bDefaultPass,
					bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch base and specular colors
	float3 sDiffColor = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.V.TexCoord0, g_diffuse).rgb;
    	
	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor);

    OUT.col.rgb = diffuse;
	if (i_bDefaultPass)
	{
		float3 emissive = Tex2DCombine(hasEmissiveMap, emissiveMap, IN.V.TexCoord0, g_emissive * g_emissiveIntensity).rgb;
		OUT.col.rgb += emissive + envmap_approximation(g_diffuseFactor).rgb;	
	}
	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput singleLightPS_Tess(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform Texture2D NormalMap,
					uniform LightInfo i_Light, 
					uniform bool i_bDefaultPass,
					bool vFace : SV_ISFRONTFACE)
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, DiffuseMap, NormalMap, i_Light, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform Texture2D NormalMap,
					uniform LightInfo i_Light, 
					uniform bool i_bDefaultPass,
					bool vFace : SV_ISFRONTFACE)
{
	return singleLightPS( IN, DiffuseMap, NormalMap, i_Light, i_bDefaultPass, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform Texture2D NormalMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch base and specular colors
	float3 sDiffColor = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.V.TexCoord0, g_diffuse).rgb;

	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor);

    OUT.col.rgb = diffuse;
	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput projLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform Texture2D NormalMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return projLightPS( IN, diffuseMap, normalMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform Texture2D NormalMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace : SV_ISFRONTFACE )
{
	return projLightPS( IN, diffuseMap, normalMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D NormalMap,
		uniform Texture2D DiffuseMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap, bool vFace : SV_ISFRONTFACE
) {
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,0);

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;
    
    float3 mask = g_bHasMask ? glowMask.Sample( AnisoWrapSampler, IN.V.TexCoord0).rgb : float3(1,1,1);
    if (g_bConstGlow)
    {
		OUT.col.rgb = mask;
    }
    else //if (mask.r+mask.g+mask.b > 0)
    {
		IncidentLight light;
		if (g_bProjLt)
			IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, light);
		else
			IlluminatePointLight(IN.V.WorldPos, i_Light, light);
		
		float3 lightDir = normalize(light.L);
		float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);

		// this material is all diffuse so lets just glow the diffuse lighting.    
		float3 sDiffColor = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.V.TexCoord0, g_diffuse).rgb;
	        
		//fetch bump normal
		float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

		OUT.col.rgb = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor) * mask;
	}
    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D NormalMap,
		uniform Texture2D DiffuseMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, NormalMap, DiffuseMap, i_Light, i_ProjLight,	ProjTextureMap,	ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D NormalMap,
		uniform Texture2D DiffuseMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap, bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, NormalMap, DiffuseMap, i_Light, i_ProjLight,	ProjTextureMap,	ProjShadowMap, vFace );
}


pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN,
		  uniform Texture2D NormalMap, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;
    
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
//	float4 diff = tex2Dlod( diffuseMap, float4( IN.TexCoord0.xy, 0, 0));

	float4 diff = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.V.TexCoord0, g_diffuse*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}

	float3 emissive = Tex2DCombine(hasEmissiveMap, emissiveMap, IN.V.TexCoord0, g_emissive * g_emissiveIntensity).rgb;
	OUT.col.rgb = g_IsolateReflection ? 0 : emissive + diff.rgb;

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput iblPS_Tess( TANGENT_VERTEX_OUTPUT IN,
		  uniform Texture2D NormalMap, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return iblPS( IN, NormalMap, vFace );
}

pixelOutput iblPS_Default( TANGENT_VERTEX_OUTPUT IN,
		  uniform Texture2D NormalMap, bool vFace : SV_ISFRONTFACE )
{
	return iblPS( IN, NormalMap, vFace );
}

/*************/

technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS_##PassName(diffuseMap, normalMap, \
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
		PixelShader = compile ps_5_0 singleLightPS_##PassName(diffuseMap, normalMap, \
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, normalMap, \
					g_lightInfo, g_projLight, \
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, normalMap, \
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, normalMap, \
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, normalMap, \
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
		PixelShader = compile ps_5_0 glowPS_##PassName(normalMap, diffuseMap, \
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
		SetPixelShader(CompileShader(ps_5_0, DOFPrepTrans_PS( hasTransparencyMap, transparencyMap, g_transparency )));\
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
		PixelShader = compile ps_5_0 iblPS_##PassName(normalMap);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
}
/***************************** eof ***/
