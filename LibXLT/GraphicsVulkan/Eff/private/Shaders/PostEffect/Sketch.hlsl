//////////////////////////////////////////////////////////////////////////////
// Converted from Sketch.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// Sketch.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Sketch.fx
**
**      Post Effect: Create Sketch effect
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Helper.hlsli"

// Explicit registers keep the binding layout identical for every entry point.
// screenTexture must stay at t0: shdwPassPostShader binds the source image
// with PSSetShaderResources(0, ...) directly.
// Sampler states, annotations and variable defaults are in Sketch.effect.json.
// defaultSampler (s0) is declared in Helper.hlsli.

cbuffer PostEffectParams : register(b0)
{
	float g_ImageWidth;		// set by shdwPassPostShader
	float g_ImageHeight;	// set by shdwPassPostShader
	float g_width;
};

// full sized source image
Texture2D screenTexture : register(t0);

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

float3 Desaturateblur(in Texture2D Tex, in float2 uv, in int w)
{
	float3 csum = 0;
	const int bw = min(abs(w), 7);	// restrict blur width to be 7 pixels

	[unroll(15)]for (int ix = -bw; ix <= bw; ix++)
	{
		[unroll(15)]for (int iy = -bw; iy <= bw; iy++)
		{
			float2 offset = float2(ix / g_ImageWidth, iy / g_ImageHeight);
			csum += desaturate(Tex.Sample(defaultSampler, uv + offset).rgb);
		}
	}

	csum /= ((2 * bw + 1) * (2 * bw + 1));

	return csum;
}

// Simple blur filter
float4 PS(VS_OUTPUT In) : SV_TARGET 
{
	float4 c = screenTexture.Sample(defaultSampler, In.img);
	float3 d1 = saturate(desaturate(c.rgb));
	float3 db = saturate(Desaturateblur(screenTexture, In.img, g_width));
	//db = 1 - db;
	
	float result = 0;
	if ((db).x > 0)
		result = saturate(d1.x / ( db.x));
	
	return float4(result.xxx, c.a);
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/***************************** eof ***/
