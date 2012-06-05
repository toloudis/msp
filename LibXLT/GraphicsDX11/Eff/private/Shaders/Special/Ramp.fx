/*****************************************************************************
**  Ramp.fx
**
**      Create ramp texture
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

#include "../Support.h"

int gradientShape = 0;
int gradientInterpolation = 0;
float uWave = 0.0f;
float uWaveFreq = 0.5f;
float vWave = 0.0f;
float vWaveFreq = 0.5f;
float noiseOffset = 0.0f;
float noiseFreq = 0.5f;

// really a 1d texture but machstudio treats it as 2d.
bool hasGradient = false;
Texture2D gradientMap;

SamplerState gradientMapSampler
{
	FILTER = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
};

bool hasNoise = false;
Texture2D noiseMap;

SamplerState noiseMapSampler
{
	FILTER = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
/************* DATA STRUCTS **************/

struct perPixelVertexOutput {
    float4 HPosition	: SV_POSITION;
    float2 UV			: TEXCOORD0;
};

/*********** support functions ******/

/*********** vertex shader ******/
perPixelVertexOutput singleLightVS(STANDARD_VERTEX IN,
    uniform float4x4 WorldViewProj
) {
    perPixelVertexOutput OUT;

	// texture coords
    OUT.UV = IN.UV;
    
	float3 NewPos = IN.Position;
    // output position in proj space
    float4 Po = float4(NewPos, 1.0);
    OUT.HPosition = TransformVertex(Po, IN.UV, WorldViewProj);
    
    return OUT;
}

perPixelVertexOutput singleLightVS_Default(STANDARD_VERTEX Vtx) 
{
	return singleLightVS( Vtx, g_wvp);
}

/********* pixel shader ********/

pixelOutput PS_Ramp(perPixelVertexOutput IN) 
{
    pixelOutput OUT = (pixelOutput)0;
    
    float2 offsetUV = IN.UV.xy;
    if (gradientShape != 2)
		offsetUV = clamp(IN.UV.xy, 0.005, 1.0f);	// add offset to remove the edge effect

	//offsetUV.xy -= float2(noiseOffset, noiseOffset);
	offsetUV.x = offsetUV.x + sin(IN.UV.y * uWaveFreq * 90) * uWave;
	offsetUV.y = offsetUV.y + sin(IN.UV.x * vWaveFreq * 90) * vWave;
	
	if (hasNoise)
	{
		float n = noiseMap.Sample( noiseMapSampler, float2(IN.UV.x, IN.UV.y) * noiseFreq).r;
		offsetUV.xy += float2(n, n) * noiseOffset;
	}
    
    float gradientCoeff = 0;
    if (hasGradient)
    {
		if (gradientShape == 0)	// linear x shape
		{
			OUT.col = gradientMap.Sample( gradientMapSampler, float2(offsetUV.x, 0) );
		}
		else if (gradientShape == 1)	// linear y shape
		{
			OUT.col = gradientMap.Sample( gradientMapSampler, float2(offsetUV.y, 0) );
		}
		else if (gradientShape == 2)	// Circular shape
		{
			gradientCoeff = length (offsetUV.xy - float2(0.5,0.5)) * 2;
			OUT.col = (gradientCoeff >= 0.995f) ? float4(0.0, 0.0, 0.0, 1.0) : gradientMap.Sample( gradientMapSampler, float2(gradientCoeff,0));
		}
		else if (gradientShape == 3)	// Square shape
		{
			 float gradientCoeff = (length(offsetUV.x - 0.5f) >= length(offsetUV.y - 0.5)) ? 
					length(offsetUV.x - 0.5f) * 2 : length(offsetUV.y - 0.5f) * 2;
			 OUT.col = gradientMap.Sample( gradientMapSampler, float2(gradientCoeff,0));
		}
			
	}
	
    return OUT;
}

/*************/

technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 PS_Ramp();\
	}
PASS_DEFAULT(Default)
}

/***************************** eof ***/
