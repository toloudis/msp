/*****************************************************************************
**  BlackAndWhite.fx
**
**      Post Effect: Create black and white image effect
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

// full sized source image
Texture2D screenTexture;
SamplerState defaultSampler
{
    Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
    AddressU = Wrap;
    AddressV = Wrap;
};

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
	float mono = (c.r + c.g + c.b) / 3.0f;
	
	return float4(mono, mono, mono, c.a);
}

technique11 Default
{
	pass p0
	{		
		VertexShader = compile vs_5_0 VSMain();
		PixelShader = compile ps_5_0 PS();
	}
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/***************************** eof ***/
