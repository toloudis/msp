// gtBindingProto.hlsl
//
// Test shader for the descriptor-heap binding prototype (vkdBindingsTest).
// Uses DX-style registers spread over the slot ranges the MachStudio
// shaders use, compiled with the same DXC options GraphicsVulkan uses
// (-fvk-b-shift 0, -fvk-t-shift 100, -fvk-s-shift 200, -fvk-u-shift 300).

cbuffer CB0 : register(b0) { float4 scale; };
cbuffer CB1 : register(b3) { float4 bias; };
Texture2D tex0 : register(t0);
Texture2D tex1 : register(t37);
SamplerState samp : register(s13);

struct VSOut
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD0;
};

// A triangle covering the whole target.
VSOut VSMain(uint id : SV_VertexID)
{
	VSOut o;
	float2 uv = float2((id << 1) & 2, id & 2);
	o.pos = float4(uv * float2(2, -2) + float2(-1, 1), 0, 1);
	o.uv = uv;
	return o;
}

float4 PSMain(VSOut i) : SV_Target
{
	return tex0.Sample(samp, i.uv) * scale + tex1.Load(int3(0, 0, 0)) * bias;
}
