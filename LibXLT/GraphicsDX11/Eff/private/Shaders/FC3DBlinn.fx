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
  string SasEffectAuthoringSoftware = "Fusion Cinema 3D";
  string SasEffectCategory			= "/material";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "Blinn";
  string SasEffectHelp				= "This is a blinn shader with oren nayar diffuse roughness.";    
  string SasEffectRevision			= "2";  
>;

/*********** support data and functions ******/

#define FC3D 1
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
	string SasUiControl = "TextureFilePicker";
	string SasUiLabel = "Diffuse Map";
	string SasUiDescription = "diffuse color";
	string UiCategory = "Diffuse";
	string ExistVar = "hasDiffuseMap";
	int UiIndex = 2;
>
;

float g_roughness
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Roughness";
	string SasUiDescription = "diffuse roughness";
	string UiCategory = "Diffuse";
	int UiIndex = 3;
>
= 0;

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


float4 g_specular : MaterialSpecular
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Specular Color";
	string SasUiDescription = "color of specular hilight";
	string UiCategory = "Specular";
	int UiIndex = 6;
>
= {1.0f, 1.0f, 1.0f, 1.0f};

bool hasSpecularMap = false;
Texture2D specularMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Color Map";
	string SasUiDescription = "specular color";
	string UiCategory = "Specular";
	string ExistVar = "hasSpecularMap";
	int UiIndex = 7;
>
;

float g_specularPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Power";
	string SasUiDescription = "Specular power controls intensity of highlight";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 8;
> = 1.0f;

float g_IOR
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Roll Off";
	string SasUiDescription = "specular fresnel index of refraction";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 9;
> = 1.0;

float g_shininess : MaterialPower 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Shininess";
	string SasUiDescription = "specular exponent controls size of highlight";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 11;
> = 0.5f;

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
    float3 Pw = mul( g_world, Po).xyz;
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

float OrenNayarDiffuse(float3 L, float3 I, float3 N, float roughness)
{
	float sigmasq = roughness*roughness;
	float A = 1.0 - 0.5 * sigmasq/(sigmasq+0.33);
	float B = 0.45 * sigmasq/(sigmasq+0.09);
	
	float IdotN = dot(I,N);
	float LdotN = dot(L,N);
	float theta_r = acos(IdotN);
	float theta_i = acos(LdotN);
	float Bfactor = max(0, cos(theta_i-theta_r));
	if (Bfactor > 0)
	{
		float alpha = max(theta_i, theta_r);
		float beta = min(theta_i, theta_r);
		Bfactor *= sin(alpha) * tan(beta);
	}
	return A + B * Bfactor;
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

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform Texture2D NormalMap,
					uniform Texture2D SpecularMap,
		  			uniform Texture2D CubeMap,
					uniform LightInfo i_Light,
					uniform float SpecPowerScale,
					bool i_bDefaultPass,
		  			bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = 1;

	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch base and specular colors
	float3 sDiffColor = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.V.TexCoord0, g_diffuse).rgb;
	float3 sSpecColor = Tex2DCombine(hasSpecularMap, SpecularMap, IN.V.TexCoord0, g_specular).rgb;
	float sSpecPower = SpecPowerScale;
    	
	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
	
	// map 0..1 to 1..g_IOR 
	float ior = g_IOR;
	
	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor);
	if (g_roughness>0)
		diffuse *= OrenNayarDiffuse(lightDir, -worldEyeDir, worldNormal, g_roughness);
	float3 specular = BlinnSpecular(worldNormal, lightDir, -worldEyeDir, 
		light.Cls, sSpecColor, sSpecPower, ior, g_specularBias);

    OUT.col.rgb = diffuse + specular;

	if (i_bDefaultPass)
	{
		OUT.col.rgb += (g_emissive.rgb * g_emissiveIntensity) + envmap_approximation(g_diffuseFactor).rgb;	
	}
	
	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
								uniform Texture2D DiffuseMap,
								uniform Texture2D normalMap,
								uniform Texture2D SpecularMap,
  								uniform Texture2D CubeMap,
								uniform LightInfo i_Light,
								uniform float SpecPowerScale,
								uniform bool i_bDefaultPass,
								bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, DiffuseMap, normalMap, SpecularMap, CubeMap, i_Light, SpecPowerScale, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN, 
								uniform Texture2D DiffuseMap,
								uniform Texture2D normalMap,
								uniform Texture2D SpecularMap,
  								uniform Texture2D CubeMap,
								uniform LightInfo i_Light,
								uniform float SpecPowerScale,
								uniform bool i_bDefaultPass,
								bool vFace : SV_ISFRONTFACE )
{
	return singleLightPS( IN, DiffuseMap, normalMap, SpecularMap, CubeMap, i_Light, SpecPowerScale, i_bDefaultPass, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform Texture2D CubeMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform float SpecPowerScale,
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
    
	//fetch base and specular colors
	float3 sDiffColor = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.V.TexCoord0, g_diffuse).rgb;
	float3 sSpecColor = Tex2DCombine(hasSpecularMap, SpecularMap, IN.V.TexCoord0, g_specular).rgb;
	float sSpecPower = SpecPowerScale;
    	
	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
	
	// map 0..1 to 1..g_IOR 
	float ior = g_IOR;

	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor);
	if (g_roughness>0)
		diffuse *= OrenNayarDiffuse(lightDir, -worldEyeDir, worldNormal, g_roughness);
	float3 specular = BlinnSpecular(worldNormal, lightDir, -worldEyeDir, 
		light.Cls, sSpecColor, sSpecPower, ior, g_specularBias);

    OUT.col.rgb = diffuse + specular;

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput projLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							uniform Texture2D DiffuseMap,
							uniform Texture2D normalMap,
							uniform Texture2D SpecularMap,
							uniform Texture2D CubeMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform float SpecPowerScale,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return projLightPS( IN, DiffuseMap, normalMap, SpecularMap, CubeMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
							uniform Texture2D DiffuseMap,
							uniform Texture2D normalMap,
							uniform Texture2D SpecularMap,
							uniform Texture2D CubeMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform float SpecPowerScale,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE )
{
	return projLightPS( IN, DiffuseMap, normalMap, SpecularMap, CubeMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform float SpecPowerScale,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap, bool vFace : SV_ISFRONTFACE
) {
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,0);
    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D normalMap,
						uniform Texture2D SpecularMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform float SpecPowerScale,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, normalMap, SpecularMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D normalMap,
						uniform Texture2D SpecularMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform float SpecPowerScale,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, normalMap, SpecularMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN,
		  uniform Texture2D NormalMap, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = 1;
    
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

	float4 refl = 0;
	if (hasCubeMap){		
		refl = SampleEnvironmentLOD((-worldEyeDir), worldNormal, cubeMap, g_reflMapAngle*PI_DIV_180, reflBlur);
		refl *= g_reflectivity;
		refl *= fastFresnel(dot(-worldEyeDir, worldNormal), fresnelBias, fresnelPower);
	}

	float4 diff = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.V.TexCoord0, g_diffuse*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}
		
	OUT.col.rgb = g_IsolateReflection ? refl.rgb : g_emissive.rgb * g_emissiveIntensity + refl.rgb + diff.rgb;

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
		PixelShader = compile ps_5_0 singleLightPS_##PassName(diffuseMap, normalMap, specularMap, cubeMap,\
					g_lightInfo, g_shininess, true);\
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
		PixelShader = compile ps_5_0 singleLightPS_##PassName(diffuseMap, normalMap, specularMap, cubeMap,\
					g_lightInfo, g_shininess, false);\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, normalMap, specularMap, cubeMap,\
					g_lightInfo, g_projLight, g_shininess,\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, normalMap, specularMap, cubeMap,\
					g_lightInfo, g_projLight, g_shininess,\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, normalMap, specularMap, cubeMap,\
					g_lightInfo, g_projLight, g_shininess,\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, normalMap, specularMap, cubeMap,\
					g_lightInfo, g_projLight, g_shininess,\
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
		PixelShader = compile ps_5_0 glowPS_##PassName(normalMap, specularMap, \
					g_lightInfo, g_projLight, g_shininess,\
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
		PixelShader = compile ps_5_0 iblPS_##PassName(normalMap);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
}
/***************************** eof ***/
