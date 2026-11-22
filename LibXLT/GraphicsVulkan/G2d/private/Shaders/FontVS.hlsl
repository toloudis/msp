// FontVS.hlsl - GraphicsVulkan only.
//
// The font vertex shader GraphicsDX11's g2dFontUtilDX11.cpp compiles at run
// time from a string (fullscreenQuadVSSource), copied here verbatim so it can
// be compiled to SPIR-V at build time (FontVS.effect.json, EmbedShadersVk.ps1).

struct QuadVS_Input
{
    float3 Pos : SV_POSITION;
	float3 Normal	: NORMAL;
	float2 Tex : TEXCOORD0;
};

struct QuadVS_Output
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

QuadVS_Output QuadVS( QuadVS_Input Input )
{
    QuadVS_Output Output;
	Output.Pos = float4(Input.Pos.xyz, 1.0f);
	Output.Tex = Input.Tex;
    return Output;
}
