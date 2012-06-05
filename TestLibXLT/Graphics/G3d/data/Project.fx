/*****************************************************************************
**  Simple.fx
**
**      No texture implementation for shader array
**
**	Gigawatt Studios
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

#include "Support.h"

/************* UN-TWEAKABLES **************/

float4x4 worldIT : WorldIT;
float4x4 wvp : WorldViewProjection;
float4x4 world : World;
float4x4 viewIT : ViewIT;

/************* TWEAKABLES **************/

float4 eyePos : CameraPos;
float shininess : MaterialPower;
float4 ambient : MaterialAmbient;

struct ProjLightInfo
{
	float4 Pos;
	float4x4 Matrix;
};

ProjLightInfo projLight : ProjLightInfo;
texture projLightTex : ProjLightTexture;
texture projShadowMap : ProjShadowMap;

/************* DATA STRUCTS **************/

/* data from application vertex buffer */
struct appdata {
    float3 Position	: POSITION;
 //   float4 UV		: TEXCOORD0;
    float3 Normal	: NORMAL;
    float4 Color	: COLOR;
};

/* data passed from vertex shader to pixel shader */
struct vertexOutput {
    float4 HPosition	: POSITION;
    float4 diffCol	: COLOR;
};
struct projVertexOutput {
    float4 HPosition	: POSITION;
    float4 ProjTexCoord	: TEXCOORD0;
    float4 diffCol	: COLOR;
//    float4 specCol	: COLOR1;
};

/* Output pixel values */
struct pixelOutput {
  float4 col : COLOR;
};

    
/*********** vertex shader ******/


vertexOutput defaultVS(appdata IN,
    uniform float4x4 WorldViewProj
) {
    vertexOutput OUT;

    float4 Po = float4(IN.Position.xyz, 1.0);
    
    OUT.diffCol = float4(0,0,0,1);
    //OUT.specCol = float4(0,0,0,0);
    
    OUT.HPosition = mul(WorldViewProj, Po);
    
    return OUT;
}

projVertexOutput projLightVS(appdata IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 World,
    uniform float4x4 WorldIT,
    uniform ProjLightInfo ProjLight
) {
    projVertexOutput OUT;
    
    float3 Nn = mul(WorldIT, float4(IN.Normal,0)).xyz;
    Nn = normalize(Nn);

    float4 Po = float4(IN.Position.xyz, 1.0);
    
    float4 Pw = mul(World, Po);
	//float3 V = normalize(EyePos - Pw);
	float3 L = normalize(ProjLight.Pos - Pw);
	float diffComp = max(dot(Nn,L), 0);
    
    OUT.diffCol = float4(diffComp,diffComp,diffComp,1);
    //OUT.specCol = float4(0,0,0,0);
	//diffuse_contrib(SingleLight, Pw, Nn, V, SpecularPower, OUT.diffCol, OUT.specCol );
    
    OUT.ProjTexCoord = mul(ProjLight.Matrix, Pw);
    //OUT.ProjTexCoord = mul(WorldViewProj, Po);
    
    OUT.HPosition = mul(WorldViewProj, Po);
    
    return OUT;
}

/********* pixel shader ********/

sampler projSampler = sampler_state
{
    Texture   = (projLightTex);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};
sampler shadowMapSampler = sampler_state
{
    Texture   = (projShadowMap);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};

pixelOutput projLightPS(projVertexOutput IN,
		uniform sampler2D ProjTextureMap,
		uniform sampler2D ProjShadowMap
) {
    pixelOutput OUT; 
    float4 col = IN.diffCol * ((IN.ProjTexCoord.w < 0) ? 0 : tex2Dproj(ProjTextureMap, IN.ProjTexCoord));
    //float4 shadowCoeff = tex2Dproj(ProjShadowMap, IN.ProjTexCoord);
    float4 shadowCoeff = tex2Dproj(ProjShadowMap, IN.ProjTexCoord);
    OUT.col = shadowCoeff * col;
    //OUT.col.a = col.a;
    return OUT;
}


pixelOutput projLightPS_withFloatTexture(projVertexOutput IN,
		uniform sampler2D ProjTextureMap,
		uniform sampler2D ProjShadowMap
) {
    pixelOutput OUT; 
    float2 ShadowTexC = IN.ProjTexCoord.xy / IN.ProjTexCoord.w;
    float pt_depth = IN.ProjTexCoord.z / IN.ProjTexCoord.w;
    
    float shdw_depth = tex2D(ProjShadowMap, ShadowTexC);
    
    float4 col = IN.diffCol * ((IN.ProjTexCoord.w < 0) ? 0 : tex2Dproj(ProjTextureMap, IN.ProjTexCoord));
    
    if (shdw_depth < pt_depth)
    	OUT.col = float4(0.0f, 0.0f, 0.0f, 1.0f);
    else
    	OUT.col = col;
    return OUT;
}

pixelOutput defaultPS(vertexOutput IN
) {
    pixelOutput OUT; 
    OUT.col = IN.diffCol;
    return OUT;
}

/*************/

technique Default
{
	pass p0 
	{		
		VertexShader = compile vs_2_0 defaultVS(wvp);
		PixelShader = compile ps_2_0 defaultPS();
	}
}
technique ProjectedLight
{
	pass p0 
	{		
		VertexShader = compile vs_2_0 projLightVS(wvp, world, worldIT, projLight);
		PixelShader = compile ps_2_0 projLightPS(projSampler, shadowMapSampler);
	}
}


/***************************** eof ***/
