// gtStructuredRead.hlsl
//
// Test shader for structured buffers as shader inputs (vkdBindingsTest):
// two StructuredBuffers at t registers next to a texture, as the hair
// shaders read g_HairMaterials and g_HairGeometry. Pixel x of a 4x1 target
// gets values[x] + (counts[x], 0, 0, tex0's alpha).

Texture2D tex0 : register(t0);
StructuredBuffer<float4> values : register(t3);
StructuredBuffer<uint> counts : register(t7);

// A triangle covering the whole target.
float4 VSMain(uint id : SV_VertexID) : SV_Position
{
	float2 uv = float2((id << 1) & 2, id & 2);
	return float4(uv * float2(2, -2) + float2(-1, 1), 0, 1);
}

float4 PSMain(float4 pos : SV_Position) : SV_Target
{
	uint x = (uint)pos.x;
	return values[x] + float4((float)counts[x], 0, 0, tex0.Load(int3(0, 0, 0)).a);
}
