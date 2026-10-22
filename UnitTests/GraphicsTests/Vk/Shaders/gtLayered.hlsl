// gtLayered.hlsl
//
// Test shader for layered rendering (vkdContextTest): the vertex shader
// makes a triangle covering the target from SV_VertexID (no vertex
// buffer); the geometry shader emits it once per slice 0 .. g_Slices - 1
// with SV_RenderTargetArrayIndex set to the slice, and, with g_bPast set,
// once more at index g_Slices (past the last slice, as LPV's injection
// can give); the pixel shader writes the slice number into three
// targets. Compiled with GraphicsVulkan's DXC options (GraphicsTests.vcxproj).

cbuffer CB0 : register(b0)
{
	uint g_Slices;
	uint g_bPast;
	uint2 g_Pad;
};

struct VSOut
{
	float4 pos : SV_Position;
};

VSOut VSMain(uint i_Id : SV_VertexID)
{
	VSOut o;
	const float2 uv = float2((i_Id << 1) & 2, i_Id & 2);
	o.pos = float4(uv * float2(2, -2) + float2(-1, 1), 0.5, 1);
	return o;
}

struct GSOut
{
	float4 pos : SV_Position;
	nointerpolation float slice : TEXCOORD0;
	uint layer : SV_RenderTargetArrayIndex;
};

[maxvertexcount(24)]
void GSMain(triangle VSOut i_In[3], inout TriangleStream<GSOut> io_Stream)
{
	const uint count = g_Slices + (g_bPast ? 1 : 0);
	for (uint s = 0; s < count; s++)
	{
		for (uint v = 0; v < 3; v++)
		{
			GSOut o;
			o.pos = i_In[v].pos;
			o.slice = (float)s;
			o.layer = s;
			io_Stream.Append(o);
		}
		io_Stream.RestartStrip();
	}
}

struct PSOut
{
	float4 a : SV_Target0;
	float4 b : SV_Target1;
	float4 c : SV_Target2;
};

PSOut PSMain(GSOut i)
{
	PSOut o;
	o.a = float4(i.slice, 0, 0, 1);
	o.b = float4(0, i.slice + 1, 0, 1);
	o.c = float4(0, 0, i.slice * 2 + 0.5, 1);
	return o;
}
