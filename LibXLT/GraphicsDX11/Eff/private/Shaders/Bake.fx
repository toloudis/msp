/*****************************************************************************
**  Bake.fx
**
**      Bake shader is only for applying bake texture on surfaces
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "Riva Chang";
  string SasEffectAuthoringSoftware = "MachStudio";
  string SasEffectCategory			= "/material";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "Bake";
  string SasEffectHelp				= "This is a Bake shader.";    
  string SasEffectRevision			= "1";  
>;

/*********** support data and functions ******/

#include "Support.h"
#include "Tessellate.h"

float4 g_emissive : MaterialEmissive
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Emissive Color";
	string SasUiDescription = "emitted color";
	string UiCategory = "Diffuse";
	int UiIndex = 1;
>
= {1.0f, 1.0f, 1.0f, 1.0f};

float g_emissiveIntensity 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Intensity";
	string SasUiDescription = "Emissive Intensity";
	string UiCategory = "Diffuse";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 2;
> = 1.0f;

bool hasEmissiveMap = false;
Texture2D emissiveMap : EmissiveTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Emissive Map";
	string SasUiDescription = "emissive color";
	string UiCategory = "Diffuse";
	string ExistVar = "hasEmissiveMap";
	int UiIndex = 3;
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
    float4 Po = float4(NewPos + In.Normal*g_glowSize, 1.0);
    
    // transform position to world space and get vector to light
    float3 Pw = mul( g_world, Po).xyz;
    OUT.WorldPos = Pw;

	OUT.WorldTan = TransformTangents( g_world, In.T, In.B, In.Normal );

    return OUT;
}

TANGENT_VERTEX_OUTPUT VS_Default( STANDARD_VERTEX Vtx, out float ClipDist : SV_ClipDistance0 ) 
{
	TANGENT_VERTEX_OUTPUT OUT;
	OUT.V = TangentVS( Vtx );

    float4 Po = float4( Vtx.Position + Vtx.Normal*g_glowSize, 1.0);
    OUT.HPosition = TransformVertex( Po, Vtx.UV, g_wvp );
	OUT.ScreenPos = float3(0,0,0);//not used

	ClipDist = ClipWorldPos( OUT.V.WorldPos );

	return OUT;
}

TANGENT_TESS_OUTPUT VS_Tess( STANDARD_VERTEX Vtx ) 
{
	TANGENT_TESS_OUTPUT OUT;
	OUT.V = TangentVS( Vtx );

	return OUT;
}

/********* pixel shader ********/
//SamplerState DefaultSampler
//{
    //Filter = ANISOTROPIC;
	//MaxAnisotropy = 16;
    //AddressU = Clamp;
    //AddressV = Clamp;
//};

pixelOutput PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	//OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, transparency).r;
	//if( g_AlphaTestRef >= OUT.col.a ) discard;
	OUT.col.a = 1.0f;

	//float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace ); //GetNormal( IN.V, vFace );

	//fetch base and specular colors
	//float3 sColor = Tex2DCombine(hasEmissiveMap, emissiveMap, IN.V.TexCoord0, g_emissive).rgb;
	float3 sColor = 0;
	if (hasEmissiveMap)
		sColor = emissiveMap.Sample(g_DefaultSampler, IN.V.TexCoord0).rgb;
    	
	OUT.col.rgb = sColor;
	
    return OUT;
}

pixelOutput PS_Tess( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return PS( IN, vFace );
}

pixelOutput PS_Default( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	return PS( IN, vFace );
}
/*************/

technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 VS_##PassName();\
		PixelShader = compile ps_5_0 PS_##PassName();\
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
		VertexShader = NULL;\
		PixelShader = NULL;\
	}
}

technique11 ProjectedLight
{
#define PASS_PROJECTEDLIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = NULL;\
		PixelShader = NULL;\
	}
}
technique11 ProjectedLightSuperSample
{
#define PASS_PROJECTEDLIGHTSS(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = NULL;\
		PixelShader = NULL;\
	}
}
technique11 ProjectedLightSuperSample2
{
#define PASS_PROJECTEDLIGHTSS2(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = NULL;\
		PixelShader = NULL;\
	}
}
technique11 ProjectedLightSuperSample3
{
#define PASS_PROJECTEDLIGHTSS3(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = NULL;\
		PixelShader = NULL;\
	}
}

technique11 Glow
{
#define PASS_GLOW(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = NULL;\
		PixelShader = NULL;\
	}
}
technique11 DOFPrep
{
#define PASS_DOFPREP(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = NULL;\
		PixelShader = NULL;\
	}
PASS_DOFPREP(Default)
PASS_DOFPREP(Tess)
}
technique11 Matte
{
#define PASS_MATTE(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = NULL;\
		PixelShader = NULL;\
	}
PASS_MATTE(Default)
PASS_MATTE(Tess)
}
technique11 Environment
{
#define PASS_ENVIRONMENT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 VS_##PassName();\
		PixelShader = compile ps_5_0 PS_##PassName();\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
}


//
//technique11 SingleLight
//{
//#define PASS_SINGLELIGHT(PassName)	\
	//pass P##PassName			\
	//{						\
		//VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		//PixelShader = compile ps_5_0 singleLightPS_##PassName(\
					//g_lightInfo, g_shininess, g_reflectivity, g_transparency, false);\
		//TANGENT_HULL_AND_DOMAIN_##PassName\
	//}
//PASS_SINGLELIGHT(Default)
//PASS_SINGLELIGHT(Tess)
//}
//
//technique11 ProjectedLight
//{
//#define PASS_PROJECTEDLIGHT(PassName)	\
	//pass P##PassName			\
	//{						\
		//VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		//PixelShader = compile ps_5_0 projLightPS_##PassName(\
					//g_lightInfo, g_projLight, g_shininess, g_reflectivity, g_transparency,\
						//BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW);\
		//TANGENT_HULL_AND_DOMAIN_##PassName\
	//}
//PASS_PROJECTEDLIGHT(Default)
//PASS_PROJECTEDLIGHT(Tess)
//}
//technique11 ProjectedLightSuperSample
//{
//#define PASS_PROJECTEDLIGHTSS(PassName)	\
	//pass P##PassName			\
	//{						\
		//VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		//PixelShader = compile ps_5_0 projLightPS_##PassName(\
					//g_lightInfo, g_projLight, g_shininess, g_reflectivity, g_transparency,\
						//BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED);\
		//TANGENT_HULL_AND_DOMAIN_##PassName\
	//}
//PASS_PROJECTEDLIGHTSS(Default)
//PASS_PROJECTEDLIGHTSS(Tess)
//}
//technique11 ProjectedLightSuperSample2
//{
//#define PASS_PROJECTEDLIGHTSS2(PassName)	\
	//pass P##PassName			\
	//{						\
		//VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		//PixelShader = compile ps_5_0 projLightPS_##PassName(\
					//g_lightInfo, g_projLight, g_shininess, g_reflectivity, g_transparency,\
						//BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH);\
		//TANGENT_HULL_AND_DOMAIN_##PassName\
	//}
//PASS_PROJECTEDLIGHTSS2(Default)
//PASS_PROJECTEDLIGHTSS2(Tess)
//}
//technique11 ProjectedLightSuperSample3
//{
//#define PASS_PROJECTEDLIGHTSS3(PassName)	\
	//pass P##PassName			\
	//{						\
		//VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		//PixelShader = compile ps_5_0 projLightPS_##PassName(\
					//g_lightInfo, g_projLight, g_shininess, g_reflectivity, g_transparency,\
						//BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH);\
		//TANGENT_HULL_AND_DOMAIN_##PassName\
	//}
//PASS_PROJECTEDLIGHTSS3(Default)
//PASS_PROJECTEDLIGHTSS3(Tess)
//}
//
//technique11 Glow
//{
//#define PASS_GLOW(PassName)	\
	//pass P##PassName			\
	//{						\
		//VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		//PixelShader = compile ps_5_0 glowPS_##PassName(\
					//g_lightInfo, g_projLight, g_shininess );\
		//TANGENT_HULL_AND_DOMAIN_##PassName\
	//}
//PASS_GLOW(Default)
//PASS_GLOW(Tess)
//}
//technique11 DOFPrep
//{
//#define PASS_DOFPREP(PassName)	\
	//pass P##PassName			\
	//{						\
		//SetVertexShader(CompileShader(vs_5_0, DOFPrepVS_##PassName()));\
		//SetPixelShader(CompileShader(ps_5_0, DOFPrepTrans_PS( hasTransparencyMap, transparencyMap, g_transparency )));\
		//DOF_HULL_AND_DOMAIN_##PassName\
	//}
//PASS_DOFPREP(Default)
//PASS_DOFPREP(Tess)
//}
//technique11 Matte
//{
//#define PASS_MATTE(PassName)	\
	//pass P##PassName			\
	//{						\
		//VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		//PixelShader = compile ps_5_0 simpleMattePS();\
		//TANGENT_HULL_AND_DOMAIN_##PassName\
	//}
//PASS_MATTE(Default)
//PASS_MATTE(Tess)
//}
//technique11 Environment
//{
//#define PASS_ENVIRONMENT(PassName)	\
	//pass P##PassName			\
	//{						\
		//VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		//PixelShader = compile ps_5_0 iblPS_##PassName();\
		//TANGENT_HULL_AND_DOMAIN_##PassName\
	//}
//PASS_ENVIRONMENT(Default)
//PASS_ENVIRONMENT(Tess)
//}
/***************************** eof ***/
