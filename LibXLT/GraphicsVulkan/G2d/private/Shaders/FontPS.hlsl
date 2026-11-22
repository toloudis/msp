// FontPS.hlsl - GraphicsVulkan only.
//
// The font pixel shader GraphicsDX11's g2dFontUtilDX11.cpp compiles at run
// time from a string (fullscreenQuadPSSource), copied here verbatim so it can
// be compiled to SPIR-V at build time (FontPS.effect.json, EmbedShadersVk.ps1).
// Its resources have no register(): DXC gives them fxc's (t0, s0, b0).

Texture2D fontMap;
SamplerState DefaultSampler
{
	FILTER = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};
struct QuadVS_Output
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};
struct pixelOutput {
	float4 col : SV_TARGET;
};
cbuffer cb0
{
	float4 g_Color;
};
pixelOutput QuadPS( QuadVS_Output Input )
{
	float4 color = g_Color;
	color *= fontMap.Sample(DefaultSampler, Input.Tex);
	/*THIS ALPHA ONLY WORKS WITH SATURATED COLORS - NEED ALPHA IN THE FONT TEXTURE*/
	color.a = max(color.r, max(color.g, color.b));
	return (pixelOutput)(color);
}
