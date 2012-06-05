/*****************************************************************************
**  LightGlow.fx
**
**      Glowing light shaft that does depth test to see if the light is 
**		occluded by anything.
**
** Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

#include "..\Support.h"
#include "..\Lighting.h"

// textures
bool hasDiffuseTexture = false;
texture2D diffuseTexture;

float edgeFuzzCutoff = 0.3f;
float distFalloffStart = 20.0f;
float distFalloffEnd = 50.0f;
float glowAlpha = 0.35f;
//float4 lightColor = float4(1,1,1,1);
//float4 lightWorldPos = float4(0,0,0,1);
bool useShadow = false;

/************* DATA STRUCTS **************/

/* data passed from vertex shader to pixel shader */
struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
    float2 TexCoord0	: TEXCOORD0;
	float3 LightVector	: TEXCOORD1;
    float3 Falloff		: COLOR0;
    float3 WorldEyeDir	: TEXCOORD4;
    float3 WorldNormal	: TEXCOORD5;
    float3 WorldPos		: TEXCOORD6;
};

/*********** support functions ******/

/*********** vertex shader ******/
DOFvertexOutput DOFPrep_VS(STANDARD_VERTEX IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldView)
{
    DOFvertexOutput OUT;
    // output position in proj space
    float4 Po = float4(IN.Position.xyz, 1.0);
    OUT.HPosition = mul(WorldViewProj, Po);
    OUT.ViewSpacePos = mul(WorldView, Po);
	OUT.TexCoord0 = mul(g_uvTransform, float4(IN.UV,0,1)).xy;
    return OUT;
}

VertexOutput projLightVS(STANDARD_VERTEX IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldIT,
    uniform float4x4 World,
    uniform ProjLightInfo ProjLight,
    uniform LightInfo SingleLight,
    uniform float4 EyePos,
	out float ClipDist : SV_ClipDistance0
) {
    VertexOutput OUT;
    
    // output position in proj space
    float4 Po = float4(IN.Position.xyz, 1.0);
    OUT.HPosition = mul(WorldViewProj, Po);
    
    // get normal for reflection vector
    //float3 Nn = mul(WorldIT, float4(IN.Normal,0)).xyz;
    //Nn = normalize(Nn);
    
    // transform position to world space and get vector to light
    float4 Pw4 = mul(World, Po);
    float3 Pw = Pw4.xyz;
    OUT.WorldPos = Pw;
    float3 Ln = normalize(SingleLight.Pos.xyz - mul(Pw, SingleLight.Pos.w));

    // attenuation in world space
	OUT.Falloff = attenuation(Pw, SingleLight);
	
	OUT.LightVector = Ln;
	
	// send world eye direction to pixel shader for reflection computation
	float3 V = normalize(EyePos.xyz - Pw);
	OUT.WorldEyeDir = (V);
	
	OUT.WorldNormal = normalize(mul((float3x3)WorldIT, IN.Normal));
	
	// decal and bump texture coords
    OUT.TexCoord0 = IN.UV;

	ClipDist = ClipWorldPos( Pw );
	
    return OUT;
}

/********* pixel shader ********/
SamplerState decalSampler
{
    FILTER = ANISOTROPIC;
};

pixelOutput projLightPS( VertexOutput IN,
		uniform Texture2D DiffuseMap,
		uniform float4 lDiffColor,
		uniform Texture2D ProjShadowMap,
		uniform float3 lightSrcInfo,
		uniform int nBlockerSamples, uniform int nShadowSamples)
{
    pixelOutput OUT; 
    
	//fetch base and specular colors
	//float4 sDiffColor = tex2D(DiffuseMap, IN.TexCoord0);
	//float4 sDiffColor = float4(1,1,1,1);
	float4 sDiffColor = Tex2DCombine(hasDiffuseTexture, DiffuseMap, IN.TexCoord0, lDiffColor);

	// smooth sides of cone
	float edgeFuzz = smoothstep(0.05, edgeFuzzCutoff, abs(dot(IN.WorldEyeDir, IN.WorldNormal)));

	// transparency falloff with distance
	// full brightness up to 20, then falloff from 20 to 50, then nothing from 50 on up to light range
	float distanceFuzz = 1 - smoothstep(distFalloffStart, distFalloffEnd, distance(IN.WorldPos, lightSrcInfo));
    	
    // shadow testing
    float is_lit = 1;
    if (useShadow)
    {
		float4 projTexCoord = mul(g_projLight.Matrix, float4(IN.WorldPos,1));
		if (projTexCoord.z >= 0)
		{
			float3 L = g_projLight.Pos.xyz - IN.WorldPos;
			float d = length(L);

			float4 shadowCoeff = 1;
			if (g_bHasShadowMap)
			{
				//shadowCoeff = SampleShadowMap(ProjShadowMap, projTexCoord, nShadowSamples, g_projLight.LightSize).x;
				shadowCoeff = PCSS(ProjShadowMap, projTexCoord, nBlockerSamples, nShadowSamples);
			}
			is_lit = shadowCoeff.x;
		}
	}

	// note 0.35 is the "full brightness" alpha
	float alpha = is_lit*glowAlpha*edgeFuzz*distanceFuzz;

	//alpha test
	if( g_AlphaTestRef >= alpha ) discard;

	OUT.col = float4(sDiffColor.r, sDiffColor.g, sDiffColor.b, 1.0f) * alpha;
    return OUT;
}

pixelOutput DOFPrep_LightCone_PS(DOFvertexOutput IN) 
{
    pixelOutput OUT; 
    clip(-1);
    OUT.col = float4(0.5,0.5,0.5,0.5);
    return OUT;
}

/*************/

technique11 ProjectedLight
{
	pass p0 
	{		
		VertexShader = compile vs_5_0 projLightVS(g_wvp,g_worldIT,
					g_world, g_projLight, g_lightInfo, g_eyePos);
		PixelShader = compile ps_5_0 projLightPS(diffuseTexture, 
					g_lightInfo.Diffuse, 
						projShadowMap, g_lightInfo.Pos.xyz, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW);
	}
}
technique11 ProjectedLightSuperSample
{
	pass p0 
	{		
		VertexShader = compile vs_5_0 projLightVS(g_wvp,g_worldIT,
					g_world, g_projLight, g_lightInfo, g_eyePos);
		PixelShader = compile ps_5_0 projLightPS(diffuseTexture, 
					g_lightInfo.Diffuse, 
						projShadowMap, g_lightInfo.Pos.xyz, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED);
	}
}
technique11 ProjectedLightSuperSample2
{
	pass p0 
	{		
		VertexShader = compile vs_5_0 projLightVS(g_wvp,g_worldIT,
					g_world, g_projLight, g_lightInfo, g_eyePos);
		PixelShader = compile ps_5_0 projLightPS(diffuseTexture, 
					g_lightInfo.Diffuse, 
						projShadowMap, g_lightInfo.Pos.xyz, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH);
	}
}

technique11 ProjectedLightSuperSample3
{
	pass p0 
	{		
		VertexShader = compile vs_5_0 projLightVS(g_wvp,g_worldIT,
					g_world, g_projLight, g_lightInfo, g_eyePos);
		PixelShader = compile ps_5_0 projLightPS(diffuseTexture, 
					g_lightInfo.Diffuse, 
						projShadowMap, g_lightInfo.Pos.xyz, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH);
	}
}

technique11 DOFPrep
{
	pass p0 
	{		
		VertexShader = compile vs_5_0 DOFPrep_VS(g_wvp,g_wv);
		PixelShader = compile ps_5_0 DOFPrep_LightCone_PS();
	}
		
}
/***************************** eof ***/
