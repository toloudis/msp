//////////////////////////////////////////////////////////////////////////////
// Converted from Sepia.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// Sepia.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Sepia.fx
**
**      Post Effect: Sepia toning
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

// Explicit registers keep the binding layout identical for every entry point.
// screenTexture must stay at t0: shdwPassPostShader binds the source image
// with PSSetShaderResources(0, ...) directly.
// Sampler states, annotations and variable defaults are in Sepia.effect.json.

cbuffer PostEffectParams : register(b0)
{
	float4 g_Color1;
};
//float4 g_Color1 = float4(0.6353f, 0.5412f, 0.3961f, 1.0f);

// full sized source image
Texture2D screenTexture : register(t0);
// sampler state is described in Sepia.effect.json
SamplerState defaultSampler : register(s0);

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
	
	// desaturate image
	float mono = ((c.r + c.g + c.b) / 3.0f);
	
	//// Create mask
	//float3 mask = g_Color1.rgb * ( 1.0f - mono );
	//
	//// Color blend to desaturated image
	//float3 finalColor = (mono).xxx * mask;
	
	return float4(g_Color1.rgb * mono, c.a);
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/***************************** eof ***/
