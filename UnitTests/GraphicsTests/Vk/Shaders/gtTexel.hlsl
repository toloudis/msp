// gtTexel.hlsl
//
// Test shader for the texture parity tests: a triangle over the whole
// target (no vertex buffer), each pixel the texel at its position of
// g_Tex, fetched exactly (Load: no filtering), all four channels. So a
// target the size of a mip shows that mip as a shader reads it.
//
// - Vulkan (this file as compiled by GraphicsTests.vcxproj): a 2D view of
//   one mip of one layer (vkdImage::Subresource)
// - GT_DX11: a Texture2DArray view (D3D11 can't view one layer of an
//   array or cube as a Texture2D); g_Select.x is the layer, .y the mip
// - GT_VOLUME (gtTexel3D.hlsl): a 3D texture; g_Select.x is the slice,
//   .y the mip

cbuffer CB0 : register(b0) { int4 g_Select; };

#if defined(GT_VOLUME)
Texture3D<float4> g_Tex : register(t0);
#elif defined(GT_DX11)
Texture2DArray<float4> g_Tex : register(t0);
#else
Texture2D<float4> g_Tex : register(t0);
#endif

float4 VSMain(uint i_Id : SV_VertexID) : SV_Position
{
	//	(-1,-1), (-1,3), (3,-1): clockwise, the front face with D3D's defaults
	const float2 corners[3] = { float2(-1, -1), float2(-1, 3), float2(3, -1) };
	return float4(corners[i_Id], 0, 1);
}

float4 PSMain(float4 i_Pos : SV_Position) : SV_Target
{
	const int2 xy = int2(i_Pos.xy);
#if defined(GT_VOLUME)
	return g_Tex.Load(int4(xy, g_Select.x, g_Select.y));
#elif defined(GT_DX11)
	return g_Tex.Load(int4(xy, g_Select.x, g_Select.y));
#else
	return g_Tex.Load(int3(xy, 0));
#endif
}
