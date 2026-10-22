// FullscreenQuadVS.hlsl - GraphicsVulkan only.
//
// The full-screen quad vertex shader GraphicsDX11's g2dFullscreenQuad.cpp
// compiles at run time from a string (fullscreenQuadVSSource), copied here
// verbatim so it can be compiled to SPIR-V at build time
// (FullscreenQuad.effect.json, EmbedShadersVk.ps1).

struct QuadVS_Input
{
    float4 Pos : SV_POSITION;
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
    Output.Pos = Input.Pos;
	Output.Tex = Input.Tex;
    return Output;
}
