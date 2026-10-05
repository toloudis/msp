//////////////////////////////////////////////////////////////////////////////
// Converted from PostAlphaMatte.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// PostAlphaMatte.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  PostAlphaMatte.fx
**
**      Alpha channel renderer
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

// Explicit registers keep the binding layout identical for every entry point
// (and map directly onto a DX12 root signature / Vulkan descriptor set).

// full scene image
Texture2D tSource : register(t0);

// sampler state is described in PostAlphaMatte.effect.json
SamplerState g_SourceSampler : register(s0);

cbuffer AlphaPostParams : register(b0)
{
	// 1/image resolution of full size image
	float2 pixelSize;
};

struct VS_OUTPUT
{
   float4 vPos: SV_POSITION;
   float2 t0: TEXCOORD0;
};

VS_OUTPUT AlphaPost_VS(float4 Pos: SV_POSITION, float2 UV : TEXCOORD0)
{
   VS_OUTPUT Out;

   // Clean up inaccuracies
   //Pos.xy = sign(Pos.xy);

   Out.vPos = Pos;//float4(Pos.xy, 0, 1);
   Out.t0 = 0.5 + 0.5*float2(Pos.x, -Pos.y);
   //Out.img.x = 0.5 * (1 + Pos.x);
   //Out.img.y = 0.5 * (1 - Pos.y);

   return Out;
}
float4 AlphaPost_PS(VS_OUTPUT v_in)  : SV_TARGET
{
	float4 cOut;
	// sample the image
	cOut = tSource.Sample(g_SourceSampler, v_in.t0);
//	return float4(cOut.r*cOut.a,cOut.g*cOut.a,cOut.b*cOut.a,cOut.a);
	return float4(cOut.a,cOut.a,cOut.a,cOut.a);
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/***************************** eof ***/
