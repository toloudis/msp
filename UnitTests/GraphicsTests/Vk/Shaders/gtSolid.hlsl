// gtSolid.hlsl
//
// Test shader for vkdPipelineCache and vkdContext: vertices with a position
// and a colour (input layout POSITION float3, COLOR float4), drawn as given;
// the pixel shader outputs the colour times cb0.tint. Compiled with
// GraphicsVulkan's DXC options (GraphicsTests.vcxproj).

cbuffer CB0 : register(b0) { float4 tint; };

struct VSIn
{
	float3 pos : POSITION;
	float4 color : COLOR0;
};

struct VSOut
{
	float4 pos : SV_Position;
	float4 color : COLOR0;
};

VSOut VSMain(VSIn i)
{
	VSOut o;
	o.pos = float4(i.pos, 1);
	o.color = i.color;
	return o;
}

float4 PSMain(VSOut i) : SV_Target
{
	return i.color * tint;
}
