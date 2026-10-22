/*****************************************************************************
**  vkdFormatTest.cpp
**
**      vkdFormat: every DXGI format the texture loaders and render targets
**      use maps to a Vulkan format and back; substitutions say why; block
**      sizes are right. DXGI values come from dxgiformat.h here (the test
**      may use it; GraphicsVulkan doesn't).
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif

#include <dxgiformat.h>
#include <gtest/gtest.h>

#include <string>

using namespace gvk;

namespace
{
	struct Mapping { DXGI_FORMAT m_Dxgi; VkFormat m_Vk; };

	//	What the loaders (DDS, TGA, PNG, JPEG, TIFF, HDR and DirectXTex's
	//	conversions of the legacy DDS formats) and the render targets make
	const Mapping c_Direct[] =
	{
		{ DXGI_FORMAT_R32G32B32A32_FLOAT,	VK_FORMAT_R32G32B32A32_SFLOAT },
		{ DXGI_FORMAT_R32G32B32A32_UINT,	VK_FORMAT_R32G32B32A32_UINT },
		{ DXGI_FORMAT_R32G32B32_FLOAT,		VK_FORMAT_R32G32B32_SFLOAT },
		{ DXGI_FORMAT_R16G16B16A16_FLOAT,	VK_FORMAT_R16G16B16A16_SFLOAT },
		{ DXGI_FORMAT_R16G16B16A16_UNORM,	VK_FORMAT_R16G16B16A16_UNORM },
		{ DXGI_FORMAT_R16G16B16A16_SNORM,	VK_FORMAT_R16G16B16A16_SNORM },
		{ DXGI_FORMAT_R16G16B16A16_UINT,	VK_FORMAT_R16G16B16A16_UINT },
		{ DXGI_FORMAT_R32G32_FLOAT,			VK_FORMAT_R32G32_SFLOAT },
		{ DXGI_FORMAT_R10G10B10A2_UNORM,	VK_FORMAT_A2B10G10R10_UNORM_PACK32 },
		{ DXGI_FORMAT_R10G10B10A2_UINT,		VK_FORMAT_A2B10G10R10_UINT_PACK32 },
		{ DXGI_FORMAT_R11G11B10_FLOAT,		VK_FORMAT_B10G11R11_UFLOAT_PACK32 },
		{ DXGI_FORMAT_R8G8B8A8_UNORM,		VK_FORMAT_R8G8B8A8_UNORM },
		{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,	VK_FORMAT_R8G8B8A8_SRGB },
		{ DXGI_FORMAT_R8G8B8A8_SNORM,		VK_FORMAT_R8G8B8A8_SNORM },
		{ DXGI_FORMAT_R8G8B8A8_UINT,		VK_FORMAT_R8G8B8A8_UINT },
		{ DXGI_FORMAT_R16G16_FLOAT,			VK_FORMAT_R16G16_SFLOAT },
		{ DXGI_FORMAT_R16G16_UNORM,			VK_FORMAT_R16G16_UNORM },
		{ DXGI_FORMAT_R16G16_SNORM,			VK_FORMAT_R16G16_SNORM },
		{ DXGI_FORMAT_D32_FLOAT,			VK_FORMAT_D32_SFLOAT },
		{ DXGI_FORMAT_R32_FLOAT,			VK_FORMAT_R32_SFLOAT },
		{ DXGI_FORMAT_R32_UINT,				VK_FORMAT_R32_UINT },
		{ DXGI_FORMAT_D24_UNORM_S8_UINT,	VK_FORMAT_D24_UNORM_S8_UINT },
		{ DXGI_FORMAT_D32_FLOAT_S8X24_UINT,	VK_FORMAT_D32_SFLOAT_S8_UINT },
		{ DXGI_FORMAT_R8G8_UNORM,			VK_FORMAT_R8G8_UNORM },
		{ DXGI_FORMAT_R8G8_SNORM,			VK_FORMAT_R8G8_SNORM },
		{ DXGI_FORMAT_R16_FLOAT,			VK_FORMAT_R16_SFLOAT },
		{ DXGI_FORMAT_D16_UNORM,			VK_FORMAT_D16_UNORM },
		{ DXGI_FORMAT_R16_UNORM,			VK_FORMAT_R16_UNORM },
		{ DXGI_FORMAT_R16_UINT,				VK_FORMAT_R16_UINT },
		{ DXGI_FORMAT_R8_UNORM,				VK_FORMAT_R8_UNORM },
		{ DXGI_FORMAT_R8_UINT,				VK_FORMAT_R8_UINT },
		{ DXGI_FORMAT_A8_UNORM,				VK_FORMAT_A8_UNORM },
		{ DXGI_FORMAT_R9G9B9E5_SHAREDEXP,	VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 },
		{ DXGI_FORMAT_BC1_UNORM,			VK_FORMAT_BC1_RGBA_UNORM_BLOCK },
		{ DXGI_FORMAT_BC1_UNORM_SRGB,		VK_FORMAT_BC1_RGBA_SRGB_BLOCK },
		{ DXGI_FORMAT_BC2_UNORM,			VK_FORMAT_BC2_UNORM_BLOCK },
		{ DXGI_FORMAT_BC2_UNORM_SRGB,		VK_FORMAT_BC2_SRGB_BLOCK },
		{ DXGI_FORMAT_BC3_UNORM,			VK_FORMAT_BC3_UNORM_BLOCK },
		{ DXGI_FORMAT_BC3_UNORM_SRGB,		VK_FORMAT_BC3_SRGB_BLOCK },
		{ DXGI_FORMAT_BC4_UNORM,			VK_FORMAT_BC4_UNORM_BLOCK },
		{ DXGI_FORMAT_BC4_SNORM,			VK_FORMAT_BC4_SNORM_BLOCK },
		{ DXGI_FORMAT_BC5_UNORM,			VK_FORMAT_BC5_UNORM_BLOCK },
		{ DXGI_FORMAT_BC5_SNORM,			VK_FORMAT_BC5_SNORM_BLOCK },
		{ DXGI_FORMAT_BC6H_UF16,			VK_FORMAT_BC6H_UFLOAT_BLOCK },
		{ DXGI_FORMAT_BC6H_SF16,			VK_FORMAT_BC6H_SFLOAT_BLOCK },
		{ DXGI_FORMAT_BC7_UNORM,			VK_FORMAT_BC7_UNORM_BLOCK },
		{ DXGI_FORMAT_BC7_UNORM_SRGB,		VK_FORMAT_BC7_SRGB_BLOCK },
		//	The packed 16-bit formats have the same bit layout under Vulkan's names
		{ DXGI_FORMAT_B5G6R5_UNORM,			VK_FORMAT_R5G6B5_UNORM_PACK16 },
		{ DXGI_FORMAT_B5G5R5A1_UNORM,		VK_FORMAT_A1R5G5B5_UNORM_PACK16 },
		{ DXGI_FORMAT_B4G4R4A4_UNORM,		VK_FORMAT_A4R4G4B4_UNORM_PACK16 },
		{ DXGI_FORMAT_B8G8R8A8_UNORM,		VK_FORMAT_B8G8R8A8_UNORM },
		{ DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,	VK_FORMAT_B8G8R8A8_SRGB },
	};
}

TEST(vkdFormat, DxgiRoundTrip)
{
	for (const Mapping& m : c_Direct)
	{
		const char* substitution = "not set";
		EXPECT_EQ(m.m_Vk, vkdFormatFromDxgi(m.m_Dxgi, &substitution)) << "DXGI " << m.m_Dxgi;
		EXPECT_EQ(nullptr, substitution) << "DXGI " << m.m_Dxgi << " has no substitution";
		EXPECT_EQ((uint32_t)m.m_Dxgi, vkdDxgiFromFormat(m.m_Vk)) << "VkFormat " << m.m_Vk;
	}
	//	No equivalent: unsupported, said by UNDEFINED
	EXPECT_EQ(VK_FORMAT_UNDEFINED, vkdFormatFromDxgi(DXGI_FORMAT_R1_UNORM));
	EXPECT_EQ(VK_FORMAT_UNDEFINED, vkdFormatFromDxgi(DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM));
	EXPECT_EQ(VK_FORMAT_UNDEFINED, vkdFormatFromDxgi(DXGI_FORMAT_UNKNOWN));
	EXPECT_EQ(VK_FORMAT_UNDEFINED, vkdFormatFromDxgi(12345));
	EXPECT_EQ(0u, vkdDxgiFromFormat(VK_FORMAT_UNDEFINED));
}

TEST(vkdFormat, SubstitutionsNamed)
{
	//	Vulkan has no X8 formats: the A8 format, whose alpha the loader
	//	fills with 255 (as D3D reads X8), with the reason given
	const struct { DXGI_FORMAT m_Dxgi; VkFormat m_Vk; } c_Substituted[] =
	{
		{ DXGI_FORMAT_B8G8R8X8_UNORM,		VK_FORMAT_B8G8R8A8_UNORM },
		{ DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,	VK_FORMAT_B8G8R8A8_SRGB },
	};
	for (const auto& s : c_Substituted)
	{
		const char* substitution = nullptr;
		EXPECT_EQ(s.m_Vk, vkdFormatFromDxgi(s.m_Dxgi, &substitution)) << "DXGI " << s.m_Dxgi;
		ASSERT_NE(nullptr, substitution) << "DXGI " << s.m_Dxgi;
		EXPECT_NE(std::string::npos, std::string(substitution).find("X8")) << substitution;
	}
	//	The round trip gives the A8 format back, not the X8 one
	EXPECT_EQ((uint32_t)DXGI_FORMAT_B8G8R8A8_UNORM, vkdDxgiFromFormat(VK_FORMAT_B8G8R8A8_UNORM));
}

TEST(vkdFormat, BlockSizes)
{
	struct Expect { VkFormat m_Vk; uint32_t m_Bytes, m_W, m_H; bool m_bDepth, m_bStencil, m_bCompressed, m_bSrgb; };
	const Expect c_Expect[] =
	{
		{ VK_FORMAT_R8_UNORM,				1, 1, 1, false, false, false, false },
		{ VK_FORMAT_R8G8B8A8_SRGB,			4, 1, 1, false, false, false, true },
		{ VK_FORMAT_B8G8R8A8_UNORM,			4, 1, 1, false, false, false, false },
		{ VK_FORMAT_R16_SFLOAT,				2, 1, 1, false, false, false, false },
		{ VK_FORMAT_R5G6B5_UNORM_PACK16,	2, 1, 1, false, false, false, false },
		{ VK_FORMAT_R16G16B16A16_UNORM,		8, 1, 1, false, false, false, false },
		{ VK_FORMAT_R32G32B32_SFLOAT,		12, 1, 1, false, false, false, false },
		{ VK_FORMAT_R32G32B32A32_SFLOAT,	16, 1, 1, false, false, false, false },
		{ VK_FORMAT_BC1_RGBA_UNORM_BLOCK,	8, 4, 4, false, false, true, false },
		{ VK_FORMAT_BC1_RGBA_SRGB_BLOCK,	8, 4, 4, false, false, true, true },
		{ VK_FORMAT_BC3_UNORM_BLOCK,		16, 4, 4, false, false, true, false },
		{ VK_FORMAT_BC4_UNORM_BLOCK,		8, 4, 4, false, false, true, false },
		{ VK_FORMAT_BC7_SRGB_BLOCK,			16, 4, 4, false, false, true, true },
		{ VK_FORMAT_D32_SFLOAT,				4, 1, 1, true, false, false, false },
		{ VK_FORMAT_D24_UNORM_S8_UINT,		4, 1, 1, true, true, false, false },
		{ VK_FORMAT_D32_SFLOAT_S8_UINT,		8, 1, 1, true, true, false, false },
		{ VK_FORMAT_D16_UNORM,				2, 1, 1, true, false, false, false },
	};
	for (const Expect& e : c_Expect)
	{
		vkdFormatInfo info = vkdFormatInfoOf(e.m_Vk);
		EXPECT_EQ(e.m_Bytes, info.m_BlockBytes) << "VkFormat " << e.m_Vk;
		EXPECT_EQ(e.m_W, info.m_BlockWidth) << "VkFormat " << e.m_Vk;
		EXPECT_EQ(e.m_H, info.m_BlockHeight) << "VkFormat " << e.m_Vk;
		EXPECT_EQ(e.m_bDepth, info.m_bDepth) << "VkFormat " << e.m_Vk;
		EXPECT_EQ(e.m_bStencil, info.m_bStencil) << "VkFormat " << e.m_Vk;
		EXPECT_EQ(e.m_bCompressed, info.m_bCompressed) << "VkFormat " << e.m_Vk;
		EXPECT_EQ(e.m_bSrgb, info.m_bSrgb) << "VkFormat " << e.m_Vk;
	}
	//	Every format in the DXGI table has its size
	for (const Mapping& m : c_Direct)
	{
		EXPECT_NE(0u, vkdFormatInfoOf(m.m_Vk).m_BlockBytes) << "VkFormat " << m.m_Vk;
	}
	EXPECT_EQ(0u, vkdFormatInfoOf(VK_FORMAT_UNDEFINED).m_BlockBytes);
}
