// TexturedQuadPS.hlsl - GraphicsVulkan only.
//
// The textured-quad pixel shader GraphicsDX11's g2dFullscreenQuad.cpp
// compiles at run time from a string (texturedQuadPSSource), copied here
// verbatim so it can be compiled to SPIR-V at build time
// (TexturedQuad.effect.json, EmbedShadersVk.ps1).

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

float4 TexturedQuadPS( float4 Pos : SV_POSITION, float2 Tex : TEXCOORD0 ) : SV_TARGET
{
    return float4(g_Texture.Sample(g_Sampler, Tex).rgb, 1.0f);
}
