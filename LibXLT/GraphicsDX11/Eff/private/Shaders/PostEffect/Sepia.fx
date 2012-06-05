/*****************************************************************************
**  Sepia.fx
**
**      Post Effect: Sepia toning
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
    AddressU = WRAP;
    AddressV = WRAP;
};

float4 g_Color1
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Sepia Color";
	string SasUiDescription = "Sepia base color";
	string UiCategory = "Properties";
	int UiIndex = 1;
>
= {0.6353f, 0.5412f, 0.3961f, 1.0f};
//float4 g_Color1 = float4(0.6353f, 0.5412f, 0.3961f, 1.0f);

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
