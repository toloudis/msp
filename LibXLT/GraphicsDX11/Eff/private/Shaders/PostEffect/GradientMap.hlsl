//////////////////////////////////////////////////////////////////////////////
// Converted from GradientMap.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// GradientMap.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  GradientMap.fx
**
**      Post Effect 
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

// Explicit registers keep the binding layout identical for every entry point.
// screenTexture must stay at t0: shdwPassPostShader binds the source image
// with PSSetShaderResources(0, ...) directly.
// Sampler states, annotations and variable defaults are in GradientMap.effect.json.

cbuffer PostEffectParams : register(b0)
{
	bool hasGradientMap;
};

// full sized source image
Texture2D screenTexture : register(t0);
Texture2D gradientMap : register(t1);

// sampler states are described in GradientMap.effect.json
SamplerState defaultSampler : register(s0);
SamplerState gradientSampler : register(s1);

struct VS_OUTPUT
{
   float4 Pos: SV_POSITION;
   float2 img: TEXCOORD0;
};

VS_OUTPUT VSMain(float4 Pos: SV_POSITION, float2 UV : TEXCOORD0 )
{
   VS_OUTPUT Out;

   // Clean up inaccuracies
   Pos.xy = sign(Pos.xy);

   Out.Pos = float4(Pos.xy, 0, 1);
   Out.img.x = 0.5 * (1 + Pos.x);
   Out.img.y = 0.5 * (1 - Pos.y);

   return Out;
}

// Simple blur filter
float4 PS(VS_OUTPUT In) : SV_TARGET 
{
	float4 c = screenTexture.Sample(defaultSampler, In.img);
	float mono = ((c.r + c.g + c.b) / 3.0f);
	float4 gc = float4(mono.xxx, c.a);
	if (hasGradientMap)
		gc = gradientMap.Sample(gradientSampler, float2(mono, 0.1f));
	
	return float4(gc.rgb, c.a);
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/***************************** eof ***/
