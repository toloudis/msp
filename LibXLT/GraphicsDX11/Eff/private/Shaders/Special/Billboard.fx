/*****************************************************************************
**  Billboard.fx
**
**      billboard: env only, no lights.
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

bool bCKActive = false;
float4 CKColor = float4(0,0,0,0);
float CKTolerance = 0.1f;
bool bCKRemoveSpill = false;
int CKSpillType = 0;
float CKSpillBias = 0.0f;
int CKEdgeBlur = 0;
float g_brightness= 1.0f;
int texWidth = 1, texHeight = 1;
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
	OUT.WorldNormal = mul((float3x3)WorldIT, IN.Normal);
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

float3 RGB_to_YCrCb(float3 c)
{
	return float3(0.299f * c.r + 0.587f * c.g + 0.114f * c.b, 
					-0.168738f * c.r - 0.331264f * c.g + 0.5f * c.b,
					0.5f * c.r - 0.418688f * c.g - 0.081312f * c.b);
}


void ChromaKey(inout float4 color, float2 UV)
{
	if (!bCKActive)
		return;
		
//bool bCKActive = false;
//float4 CKColor = float4(0,0,0,0);
//float CKTolerance = 0.1f;
//bool bCKRemoveSpill = false;
//int CKSpillType = 0;
//float CKSpillBias = 0.0f;
//int CKEdgeBlur = 0;
		
	float alpha = 1.0f, spill = 0.0f;
	bool spillG = 1 - CKSpillType, spillB = CKSpillType;
	// YCrCb range
	float3 cKey = RGB_to_YCrCb(CKColor.rgb);
	
	const int r = min(abs(CKEdgeBlur), 7);
	float sum = 0;
	
	[unroll(15)]for (int ix = -r; ix <= r; ix++)
	{
		[unroll(15)]for(int iy = -r; iy <= r; iy++)
		{
			float2 offsetUV = UV + float2((float)ix / texWidth, (float)iy / texHeight);
			float4 c = g_diffuse*g_envDiffuseColor*g_diffuseFactor;
			if (hasDiffuseMap)
				c *= diffuseMap.Sample(g_DefaultSampler, offsetUV);
			c.rgb = RGB_to_YCrCb(c.rgb);
			
			if ( sqrt((c.g - cKey.g) * (c.g - cKey.g) + (c.b - cKey.b) * (c.b - cKey.b) ) <= CKTolerance)
			{
				alpha = 0.0f;
			}
			else
			{
				alpha = 1.0f;
			}
			
			sum += alpha;
		}
	}
	
	color.a = sum / float((r * 2 + 1) * (r * 2 + 1));
	if (bCKRemoveSpill)
	{
		spill = max(0.0f, (color.g * spillG + color.b * spillB) - 
						(color.r * CKSpillBias + (1.0f - CKSpillBias) * color.b * spillG + (1.0f - CKSpillBias) * color.g * spillB));
	}
	color.g = color.g - spill * spillG;
	color.b = color.b - spill * spillB;
	
	return;
}

pixelOutput labPS( VertexOutput IN) 
{
    // alpha blend refl layer and base color
    pixelOutput OUT; 
    OUT.col = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse);
    
    ChromaKey(OUT.col, IN.TexCoord0);

	// alpha test
	if( g_AlphaTestRef >= OUT.col.a ) discard;

    return OUT;
}

pixelOutput iblPS( VertexOutput IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse).a;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

//	OUT.col = Tex2DCombine(hasDiffuseMap, diffuseSampler, IN.TexCoord0, g_diffuse);
//	return OUT;

	//fetch bump normal
	float3 worldEyeDir = normalize(IN.WorldPos - g_eyePos.xyz);
	float3 worldNormal = normalize(IN.WorldNormal);
	if (g_bDoubleSided && vFace > 0)
	{
		worldNormal = -worldNormal;
	}

	float4 diff = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse*g_envDiffuseColor*g_diffuseFactor);	
	diff *= float4(g_brightness,g_brightness,g_brightness,1);
	
	ChromaKey(diff, IN.TexCoord0);
	
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}
		
	OUT.col.rgb = g_IsolateReflection ? 0 : diff.rgb * diff.a;
	OUT.col.a = diff.a;

    return OUT;
}

float4 PSBlur
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
    
    return float4(1,0,0,1);	
}

/*************/

technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 labPS();\
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
