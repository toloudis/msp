/*****************************************************************************
**  Brushstroke.fx
**
**      brushstroke: paint brush shader
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

#include "../Support.h"

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
Texture2D diffuseMap : DiffuseTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Diffuse Map";
	string SasUiDescription = "diffuse color";
	string UiCategory = "Diffuse";
	string ExistVar = "hasDiffuseMap";
	int UiIndex = 2;
>
;

/************* DATA STRUCTS **************/

struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
    float2 TexCoord0	: TEXCOORD0;
    float2 UV			: TEXCOORD1;
    float3 WorldPos		: TEXCOORD2;
   	float3 WorldNormal	: TEXCOORD3;
};


/*********** support functions ******/
    
/*********** vertex shader ******/

VertexOutput singleLightVS(STANDARD_VERTEX IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldIT,
    uniform float4x4 World,
    uniform float GlowSize )
{
    VertexOutput OUT;

	// decal and bump texture coords
    OUT.TexCoord0 = mul(g_uvTransform, float4(IN.UV,0,1)).xy;
    OUT.UV = IN.UV;
    
	float3 NewPos = IN.Position;

    // output position in proj space
    float4 Po = float4(NewPos + IN.Normal*GlowSize, 1.0);
    OUT.HPosition = TransformVertex(Po, IN.UV, WorldViewProj);
    
    // transform position to world space and get vector to light
    float3 Pw = mul(World, Po).xyz;
    OUT.WorldPos = Pw;
	OUT.WorldNormal = mul((float3x3)WorldIT, IN.Normal).xyz;

    return OUT;
}

VertexOutput singleLightVS_Default(STANDARD_VERTEX Vtx, out float ClipDist : SV_ClipDistance0 ) 
{
	VertexOutput Out;
	Out = singleLightVS( Vtx, g_wvp,g_worldIT,
					g_world, g_glowSize );

	ClipDist = ClipWorldPos( Out.WorldPos );

	return Out;
}

/********* pixel shader ********/

SamplerState diffuseSampler
{
	FILTER = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = WRAP;
};

pixelOutput labPS( VertexOutput IN,
		  uniform Texture2D DiffuseMap) 
{
    // alpha blend refl layer and base color
    pixelOutput OUT; 
    float4 col = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.TexCoord0, g_diffuse);
	
    OUT.col.rgb = col.rgb;
    //OUT.col.rgb *= GetAO(IN.UV);

	OUT.col.a = col.a;
	
	float radius_sq = length(IN.UV.xy - float2(0.5,0.5));
	radius_sq *= radius_sq;
	//OUT.col.rgb = g_diffuse * exp(-radius_sq);
	//OUT.col.rgb *= GetAO(IN.UV);
	//OUT.col.a = exp(-radius_sq);
    OUT.col.a = col.a * exp(-radius_sq);
    return OUT;
}

pixelOutput iblPS( VertexOutput IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 
//	OUT.col = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse);
//	return OUT;

	//fetch bump normal
	float3 worldEyeDir = normalize(IN.WorldPos - g_eyePos.xyz);
	float3 worldNormal = normalize(IN.WorldNormal);
	if (g_bDoubleSided && vFace > 0)
	{
		worldNormal = -worldNormal;
	}

	float4 diff = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}
	
	OUT.col = float4(
		diff.rgb,
		diff.a );		

    //OUT.col.rgb *= GetAO(IN.UV);
	//OUT.col.a = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse).a;
	
	
	//float radius_sq = (IN.UV.xy - float2(0.5,0.5));
	//radius_sq *= radius_sq;
	//OUT.col.rgb = g_diffuse * exp(-radius_sq);
	//OUT.col.rgb *= GetAO(IN.UV);
	//OUT.col.a = exp(-radius_sq);
	OUT.col.a = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse).a;
	
    return OUT;
}

/*************/

technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 labPS(diffuseMap);\
	}
PASS_DEFAULT(Default)
//PASS_DEFAULT(Tess)
}
technique11 DOFPrep
{
#define PASS_DOFPREP(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 DOFPrepVS_##PassName();\
		PixelShader = compile ps_5_0 DOFPrep_PS();\
	}
PASS_DOFPREP(Default)
//PASS_DOFPREP(Tess)
}
technique11 Matte
{
#define PASS_MATTE(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 simpleMattePS();\
	}
PASS_MATTE(Default)
//PASS_MATTE(Tess)
}
technique11 Environment
{
#define PASS_ENVIRONMENT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 iblPS();\
	}
PASS_ENVIRONMENT(Default)
//PASS_ENVIRONMENT(Tess)
}
/***************************** eof ***/
