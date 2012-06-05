/*****************************************************************************
**  Blinn.fx
**
**      Blinn (Torrance-Sparrow) shader
**
**	Studio GPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "Daniel Toloudis";
  string SasEffectAuthoringSoftware = "MachStudio";
  string SasEffectCategory			= "/material";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "Blinn";
  string SasEffectHelp				= "This is a blinn shader with oren nayar diffuse roughness.";    
  string SasEffectRevision			= "2";  
>;

/*********** support data and functions ******/

#include "Support.h"
#include "Tessellate.h"
#include "Lighting.h"

float4 g_emissive : MaterialEmissive
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Emissive Color";
	string SasUiDescription = "emissive color";
	string UiCategory = "Diffuse";
	int UiIndex = 4;
>
= {0.0f, 0.0f, 0.0f, 1.0f};

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
	string SasUiDescription = "opacity value, 0 transparent, 1 for opaque";
	string UiCategory = "Opacity";
	int UiIndex = 13;
>
= 1.0f;

bool hasTransparencyMap = false;
Texture2D transparencyMap	: OpacityTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Opacity Texture";
	string SasUiDescription = "opacity texture";
	string UiCategory = "Opacity";
	string ExistVar = "hasTransparencyMap";
	int UiIndex = 14;
>
;

float g_reflectivity : Reflectivity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Environment Contribution";
	string SasUiDescription = "multiplier for reflection map";
	string UiCategory = "Reflection";
	int UiIndex = 15;
>
= 1.0f;

float fresnelPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel Power";
	string SasUiDescription = "fresnel exponent";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 5.0;
	int UiIndex = 16;
> = 4.0;

float fresnelBias
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel Bias";
	string SasUiDescription = "fresnel bias";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 17;
> = 0.2;

float reflBlur
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Reflection Blur";
	string SasUiDescription = "reflection blur";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 18;
>
= 0.0;
float g_reflMapAngle 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Map Angle";
	string SasUiDescription = "Rotation of reflection around Y";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 360.0;
	int UiIndex = 19;
> = 0;

bool hasCubeMap = false;
Texture2D cubeMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Reflection Map";
	string SasUiDescription = "reflection map";
	string UiCategory = "Reflection";
	string ExistVar = "hasCubeMap";
	int UiIndex = 20;
>
;

bool hasReflectFactorMap = false;
Texture2D reflectFactorMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Reflection Mask Map";
	string SasUiDescription = "reflection mask";
	string UiCategory = "Reflection";
	string ExistVar = "hasReflectFactorMap";
	int UiIndex = 21;
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
float3 Shade(IncidentLight i_Light, float3 i_LightDir, float4 i_EyePos, TANGENT_VERTEX_OUTPUT IN, float3 i_WorldNormal)
{
	return float3(1,0,0);
}



SamplerState AnisoWrapSampler
{
	FILTER = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = WRAP;
};

pixelOutput singleLightPS(	TANGENT_VERTEX_OUTPUT IN,
							uniform Texture2D NormalMap,
							uniform LightInfo i_Light,
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
    
	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
	
	OUT.col.rgb = Shade(light, lightDir, g_eyePos, IN, worldNormal);

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
								uniform Texture2D normalMap,
								uniform LightInfo i_Light,
								bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, normalMap, i_Light, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN, 
								uniform Texture2D normalMap,
								uniform LightInfo i_Light,
								bool vFace : SV_ISFRONTFACE )
{
	return singleLightPS( IN, normalMap, i_Light, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
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
    
	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );


	OUT.col.rgb = Shade(light, lightDir, g_eyePos, IN, worldNormal);

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput projLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							uniform Texture2D normalMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return projLightPS( IN, normalMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
							uniform Texture2D normalMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE )
{
	return projLightPS( IN, normalMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D NormalMap,
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
    
		//fetch bump normal
		float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

		// specular only for glow.
		light.Cld = 0;
		
		OUT.col.rgb = Shade(light, lightDir, g_eyePos, IN, worldNormal);	        
	}
    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D normalMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, normalMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D normalMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, normalMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
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

	float4 refl = 0;
	if (hasCubeMap){		
		refl = SampleEnvironmentLOD((-worldEyeDir), worldNormal, cubeMap, g_reflMapAngle*PI_DIV_180, reflBlur);
		refl *= g_reflectivity;
		refl *= fastFresnel(dot(-worldEyeDir, worldNormal), fresnelBias, fresnelPower);
		refl = Tex2DCombine(hasReflectFactorMap, reflectFactorMap, IN.V.TexCoord0, refl);
	}

	IncidentLight light;
	light.Cls = (g_reflectivity*g_envSpecularColor*g_specularFactor).rgb;
	if (g_bHasSpecularEnvMap) 
	{
		light.Cls *= SampleEnvironment((-worldEyeDir), worldNormal, 
				specularEnvMap, g_specularEnvAngle).rgb;
	}
		
	light.Cld = (g_envDiffuseColor*g_diffuseFactor).rgb;
	if (g_bHasDiffuseEnvMap)
	{
		light.Cld *= SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb;
	}
		
	float3 lightDir = -worldNormal;
	light.L = lightDir;
	
	OUT.col.rgb = Shade(light, lightDir, g_eyePos, IN, worldNormal);
	OUT.col.rgb += g_emissive.rgb * g_emissiveIntensity + refl.rgb;

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput iblPS_Tess( TANGENT_VERTEX_OUTPUT IN, uniform Texture2D normalMap, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return iblPS( IN, normalMap, vFace );
}

pixelOutput iblPS_Default( TANGENT_VERTEX_OUTPUT IN, uniform Texture2D normalMap, bool vFace : SV_ISFRONTFACE )
{
	return iblPS( IN, normalMap, vFace );
}

/*************/

technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS_##PassName(normalMap, g_lightInfo);\
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
		PixelShader = compile ps_5_0 singleLightPS_##PassName(normalMap, g_lightInfo);\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(normalMap, \
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(normalMap, \
					g_lightInfo, g_projLight, \
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(normalMap, \
					g_lightInfo, g_projLight, \
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(normalMap, \
					g_lightInfo, g_projLight, \
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
		PixelShader = compile ps_5_0 glowPS_##PassName(normalMap, \
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
		SetVertexShader(CompileShader(vs_5_0, DOFPrepVS_##PassName()));\
		SetPixelShader(CompileShader(ps_5_0, DOFPrep_PS()));\
		DOF_HULL_AND_DOMAIN_##PassName\
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
