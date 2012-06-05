/*****************************************************************************
**  Cartoon.fx
**
**      
**  John Schwab
**	Extra Large Technoloy
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "John Schwab";
  string SasEffectAuthoringSoftware = "MachStudio";
  string SasEffectCategory			= "/material";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "Cartoon";
  string SasEffectHelp				= "This is a cartoon shader.";    
  string SasEffectRevision			= "1";  
  string SupportsOutline            = "true";
>;

/*********** support data and functions ******/

#include "Support.h"
#include "Lighting.h"
#include "Tessellate.h"

float4 g_ambient : MaterialAmbient 
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Ambient";
	string SasUiDescription = "ambient color";
	string UiCategory = "Color";
	int UiIndex = 1;
> = {0.5f, 0.5f, 0.5f, 1.0f};

float4 g_midtone : MaterialMidtone 
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Midtone";
	string SasUiDescription = "midtone color";
	string UiCategory = "Color";
	int UiIndex = 2;
> = {0.5f, 0.5f, 0.5f, 1.0f};

float4 g_diffuse : MaterialDiffuse 
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Diffuse";
	string SasUiDescription = "diffuse color";
	string UiCategory = "Color";
	int UiIndex = 3;
> = {1.0f, 1.0f, 1.0f, 1.0f};

/*
float g_colorTransition0 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Ambient to Midtone";
	string SasUiDescription = "ambient to midtone";
	string UiCategory = "Color Transition";
	int UiIndex = 5;
> = 0.1f;
*/
float g_colorTransition1 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Bias";
	string SasUiDescription = "Shift Midtone transition";
	string UiCategory = "Color Band";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 4;
> = 0.5f;

float g_smoothness
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Smoothness";
	string SasUiDescription = "Controls how smooth the color band line is.";
	string UiCategory = "Color Band";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 5;
> = 0.5f;

float g_transparency : Opacity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Opacity";
	string SasUiDescription = "Opacity value, 0 transparent, 1 for opaque";
	string UiCategory = "Opacity";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 6;
> = 1.0f;

bool hasTransparencyMap = false;
Texture2D transparencyMap	: OpacityTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Opacity Texture";
	string SasUiDescription = "Opacity texture";
	string UiCategory = "Opacity";
	string ExistVar = "hasTransparencyMap";
	int UiIndex = 7;
>;

bool g_hasSpecular 
<
	string SasUiControl = "CheckBox";
	string SasUiLabel = "Enable Specular";
	string SasUiDescription = "specular enable";
	string UiCategory = "Specular";
	int UiIndex = 8;
> = false;

float4 g_specular
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Specular";
	string SasUiDescription = "specular color";
	string UiCategory = "Specular";
	int UiIndex = 9;
> = {1.0f, 1.0f, 1.0f, 1.0f};

float g_specularPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Power";
	string SasUiDescription = "Color Multiplier";
	string UiCategory = "Specular";
	float SasUiMin = 1.0;
	float SasUiMax = 10.0;
	int UiIndex = 10;
> = 1.0f;

float g_colorTransition2
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Shininess";
	string SasUiDescription = "Amount of specular highlite";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 11;
> = 0.9f;

// textures
bool hasDiffuseMap		= false;
Texture2D diffuseMap		: DiffuseTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Diffuse Texture";
	string SasUiDescription = "diffuse color";
	string UiCategory = "Color";
	string ExistVar = "hasDiffuseMap";
	int UiIndex = 12;
>;
/*
bool hasGradientMap		= false;
texture1D gradientMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Gradient Map";
	string SasUiDescription = "Gradient Map (1D)";
	string UiCategory = "Maps";
	string ExistVar = "hasGradientMap";
>;
*/
bool hasSpecularMap = false;
Texture2D specularMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Texture";
	string SasUiDescription = "specular color map";
	string UiCategory = "Specular";
	string ExistVar = "hasSpecularMap";
	int UiIndex = 13;
>;
bool hasGlossMap = false;
Texture2D glossMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Gloss Texture";
	string SasUiDescription = "specular exponent multiplier";
	string UiCategory = "Specular";
	string ExistVar = "hasGlossMap";
	int UiIndex = 14;
>;

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
    OUT.HPosition = TransformVertex(Po, Vtx.UV, g_wvp );
	OUT.ScreenPos = float3(0,0,0);

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

float4 ToonShader(
	uniform float2 texCoord,
	uniform float3 WorldPos,
	uniform float3 WorldNormal,
	uniform IncidentLight Light
	)
{
   float4 color = float4(0,0,0,0);
   
	float3 lightDir = normalize(Light.L);
	float3 worldEyeDir = normalize(WorldPos - g_eyePos.xyz);
	float3 worldNormal = normalize(WorldNormal);

   float4 diffColor = float4(Light.Cld,1);
   if( hasDiffuseMap )
   {
      diffColor *= diffuseMap.Sample( AnisoWrapSampler, texCoord );
   }
   
   float dif = saturate(dot( lightDir, worldNormal ));
   
   float spec = 0;
   if( g_hasSpecular )
   {
  	  float spec = saturate(dot(worldNormal,normalize(lightDir - worldEyeDir)));
	  float3 specClr = g_specular.rgb * g_specularPower;
	  float specTrans = g_colorTransition2;
	  if( hasSpecularMap )
	  {
		  specClr = specularMap.Sample( AnisoWrapSampler, texCoord ).rgb;
	  }
	  if( hasGlossMap )
	  {
		  specTrans = glossMap.Sample( AnisoWrapSampler, texCoord ).r;
	  }
      float interp = saturate( 1 - ((specTrans - spec) / g_smoothness) );
	  color.rgb = specClr * Light.Cls * interp;
   }      
   
   float trans0 = g_colorTransition1 * 0.25f;
   if( dif > g_colorTransition1 )
   {
      float interp = saturate(((dif - g_colorTransition1) / ((1 - g_colorTransition1) * g_smoothness )));
      diffColor *= lerp( g_midtone, g_diffuse, interp );
   }
   else if( dif > trans0 )
   {
      float interp = saturate(((dif - trans0) / ((g_colorTransition1 - trans0) * g_smoothness)));
      diffColor *= lerp( g_ambient, g_midtone, interp );
   }
   else
   {
	   diffColor *= g_ambient;
   }

   color += diffColor;
   
	return color;
}

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform LightInfo i_Light,
					uniform bool i_bDefaultPass,
					bool vFace : SV_ISFRONTFACE )
{
    pixelOutput OUT;

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);
	
	float3 bumpNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

	float3 diffuse = ToonShader( IN.V.TexCoord0, IN.V.WorldPos, bumpNormal/*IN.V.WorldTan.Z*/, light ).rgb;

    OUT.col.rgb = diffuse;
    
	if (i_bDefaultPass)
	{
		OUT.col.rgb += (g_ambient.rgb) + envmap_approximation(g_diffuseFactor).rgb;	
	}
    
	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							   uniform Texture2D DiffuseMap,
								uniform LightInfo i_Light,
								uniform bool i_bDefaultPass,
								bool vFace : SV_ISFRONTFACE  )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, DiffuseMap, i_Light, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								  uniform Texture2D DiffuseMap,
								uniform LightInfo i_Light,
								uniform bool i_bDefaultPass,
								bool vFace : SV_ISFRONTFACE  )
{
	return singleLightPS( IN, DiffuseMap, i_Light, i_bDefaultPass, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples,
		bool vFace : SV_ISFRONTFACE  )
{
    pixelOutput OUT;

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);

	float3 bumpNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

	float3 diffuse = ToonShader( IN.V.TexCoord0, IN.V.WorldPos, bumpNormal, light ).rgb;

    OUT.col.rgb = diffuse;
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
							bool vFace : SV_ISFRONTFACE   )
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
							bool vFace : SV_ISFRONTFACE   )
{
	return projLightPS( IN, DiffuseMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap, bool vFace : SV_ISFRONTFACE )
{
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

		float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace ); //GetNormal( IN.V, vFace );
	        
		OUT.col.rgb = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor) * mask;
	}
    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D DiffuseMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, DiffuseMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
						   uniform Texture2D DiffuseMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
						   bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, DiffuseMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace ); //GetNormal( IN.V, vFace );
		
	float4 diff = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.V.TexCoord0, g_diffuse*g_envDiffuseColor*g_diffuseFactor);
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

/*************/

technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS_##PassName(diffuseMap, \
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
		PixelShader = compile ps_5_0 singleLightPS_##PassName(diffuseMap, \
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, \
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap,\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap,\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap,\
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
		PixelShader = compile ps_5_0 glowPS_##PassName(diffuseMap, \
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
		PixelShader = compile ps_5_0 iblPS_##PassName();\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
}
/***************************** eof ***/
