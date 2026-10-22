// gtLink.hlsl
//
// Test shader for vkdTrimUnconsumedOutputs: VSMain writes locations 0-3,
// a clip distance and the position; PSMain takes only locations 1 and 3,
// PSAll all four. Drawn with vertex ids only (no vertex layout): a full-
// screen triangle whose colour comes from locations 1 and 3, clipped by
// SV_ClipDistance0 to the left half of the target. Compiled with
// GraphicsVulkan's DXC options (GraphicsTests.vcxproj).

struct VSOut
{
	float4 pos : SV_Position;
	[[vk::location(0)]] float4 a : TEXCOORD0;
	[[vk::location(1)]] float4 b : TEXCOORD1;
	[[vk::location(2)]] float4 c : TEXCOORD2;
	[[vk::location(3)]] float4 d : TEXCOORD3;
	float clip : SV_ClipDistance0;
};

VSOut VSMain(uint id : SV_VertexID)
{
	float2 uv = float2((id << 1) & 2, id & 2);
	VSOut o;
	o.pos = float4(uv * 2 - 1, 0, 1);
	o.a = float4(1, 0, 0, 1);
	o.b = float4(0, 0.5, 0, 1);
	o.c = float4(0, 0, 1, 1);
	o.d = float4(0.25, 0, 0.25, 0);
	o.clip = -o.pos.x;		// keeps x <= 0
	return o;
}

struct PSIn
{
	float4 pos : SV_Position;
	[[vk::location(1)]] float4 b : TEXCOORD1;
	[[vk::location(3)]] float4 d : TEXCOORD3;
};

float4 PSMain(PSIn i) : SV_Target
{
	return i.b + i.d;
}

float4 PSAll(VSOut i) : SV_Target
{
	// reads every input (DXC drops the inputs a pixel shader doesn't read)
	return i.b + i.d + (i.a + i.c) * i.pos.z;
}
