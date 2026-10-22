// gtAniso.hlsl
//
// Test shader for vkdAnisotropicTest: vertices with a clip-space position
// (projected on the CPU, so both APIs get the same numbers) and a texture
// coordinate. PSMain samples g_Tex with g_Sampler; PSLod writes the level
// of detail the sampler computes there (clamped to the mips, and
// unclamped); PSDeriv writes the texture coordinate's derivatives.
// Compiled with GraphicsVulkan's DXC options (GraphicsTests.vcxproj) and,
// for D3D11, at run time with fxc.

Texture2D<float4> g_Tex : register(t0);
SamplerState g_Sampler : register(s0);

struct VSIn
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD0;
};

struct VSOut
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD0;
};

VSOut VSMain(VSIn i)
{
	VSOut o;
	o.pos = i.pos;
	o.uv = i.uv;
	return o;
}

float4 PSMain(VSOut i) : SV_Target
{
	return g_Tex.Sample(g_Sampler, i.uv);
}

float4 PSLod(VSOut i) : SV_Target
{
	return float4(g_Tex.CalculateLevelOfDetail(g_Sampler, i.uv), g_Tex.CalculateLevelOfDetailUnclamped(g_Sampler, i.uv), 0, 1);
}

// The screen-space derivatives of the texture coordinate: (du/dx, dv/dx, du/dy, dv/dy)
float4 PSDeriv(VSOut i) : SV_Target
{
	return float4(ddx(i.uv), ddy(i.uv));
}
