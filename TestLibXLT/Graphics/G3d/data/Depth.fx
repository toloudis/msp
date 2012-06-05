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
LightInfo lightArray[8] : LightArray;

/************* TWEAKABLES **************/

float4 eyePos : CameraPos;
float shininess : MaterialPower;
float4 ambient : MaterialAmbient;

LightInfo lightInfo : LightInfo;

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
    float2 Depth : TEXCOORD0;
};

/* Output pixel values */
struct pixelOutput {
  float4 col : COLOR;
};

    
/*********** vertex shader ******/

vertexOutput multiLightVS(appdata IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldIT,
    uniform float4x4 World,
    uniform float4x4 ViewIT,
    uniform LightInfo LightArray[8],
    uniform float4 EyePos,
    uniform float SpecularPower,
    uniform float4 MaterialAmbient,
    uniform bool LightAmbientMod
) {
    vertexOutput OUT;

    float4 Po = float4(IN.Position.xyz, 1.0);
    
    //float3 Pw = mul(World, Po).xyz;
    //float D = normalize(EyePos - Pw);
    
    // Output vertex position
    float4 hpos = mul(WorldViewProj, Po);
    OUT.HPosition = hpos;
    
    OUT.Depth.xy = hpos.zw;
    
    return OUT;
}

/********* pixel shader ********/

pixelOutput labPS(vertexOutput IN
) {
    pixelOutput OUT; 
    // Depth is z / w
    OUT.col = IN.Depth.x / IN.Depth.y;
    return OUT;
}

/*************/

technique Default
{
	pass p0 
	{		
		VertexShader = compile vs_2_0 multiLightVS(wvp,worldIT,
					world,viewIT, lightArray, eyePos, shininess, ambient, false);
		PixelShader = compile ps_2_0 labPS();
	}
}


/***************************** eof ***/
