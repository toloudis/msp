/*****************************************************************************
**  vkdFormat.cpp
**
**      See vkdFormat.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif

namespace gvk
{
	namespace
	{
		enum Flags { e_Depth = 1, e_Stencil = 2, e_Compressed = 4, e_Srgb = 8 };

		//	A Vulkan format, its block size, and the DXGI_FORMAT value it
		//	stands for (0 = none). One row per Vulkan format.
		struct Format
		{
			VkFormat	m_Vk;
			uint32_t	m_Dxgi;
			uint32_t	m_Bytes;
			uint32_t	m_Block;		// block width and height
			uint32_t	m_Flags;
		};

		const Format c_Formats[] =
		{
			{ VK_FORMAT_R32G32B32A32_SFLOAT,		2,   16, 1, 0 },
			{ VK_FORMAT_R32G32B32A32_UINT,			3,   16, 1, 0 },
			{ VK_FORMAT_R32G32B32A32_SINT,			4,   16, 1, 0 },
			{ VK_FORMAT_R32G32B32_SFLOAT,			6,   12, 1, 0 },
			{ VK_FORMAT_R32G32B32_UINT,				7,   12, 1, 0 },
			{ VK_FORMAT_R32G32B32_SINT,				8,   12, 1, 0 },
			{ VK_FORMAT_R16G16B16A16_SFLOAT,		10,  8, 1, 0 },
			{ VK_FORMAT_R16G16B16A16_UNORM,			11,  8, 1, 0 },
			{ VK_FORMAT_R16G16B16A16_UINT,			12,  8, 1, 0 },
			{ VK_FORMAT_R16G16B16A16_SNORM,			13,  8, 1, 0 },
			{ VK_FORMAT_R16G16B16A16_SINT,			14,  8, 1, 0 },
			{ VK_FORMAT_R32G32_SFLOAT,				16,  8, 1, 0 },
			{ VK_FORMAT_R32G32_UINT,				17,  8, 1, 0 },
			{ VK_FORMAT_R32G32_SINT,				18,  8, 1, 0 },
			{ VK_FORMAT_D32_SFLOAT_S8_UINT,			20,  8, 1, e_Depth | e_Stencil },
			{ VK_FORMAT_A2B10G10R10_UNORM_PACK32,	24,  4, 1, 0 },
			{ VK_FORMAT_A2B10G10R10_UINT_PACK32,	25,  4, 1, 0 },
			{ VK_FORMAT_B10G11R11_UFLOAT_PACK32,	26,  4, 1, 0 },
			{ VK_FORMAT_R8G8B8A8_UNORM,				28,  4, 1, 0 },
			{ VK_FORMAT_R8G8B8A8_SRGB,				29,  4, 1, e_Srgb },
			{ VK_FORMAT_R8G8B8A8_UINT,				30,  4, 1, 0 },
			{ VK_FORMAT_R8G8B8A8_SNORM,				31,  4, 1, 0 },
			{ VK_FORMAT_R8G8B8A8_SINT,				32,  4, 1, 0 },
			{ VK_FORMAT_R16G16_SFLOAT,				34,  4, 1, 0 },
			{ VK_FORMAT_R16G16_UNORM,				35,  4, 1, 0 },
			{ VK_FORMAT_R16G16_UINT,				36,  4, 1, 0 },
			{ VK_FORMAT_R16G16_SNORM,				37,  4, 1, 0 },
			{ VK_FORMAT_R16G16_SINT,				38,  4, 1, 0 },
			{ VK_FORMAT_D32_SFLOAT,					40,  4, 1, e_Depth },
			{ VK_FORMAT_R32_SFLOAT,					41,  4, 1, 0 },
			{ VK_FORMAT_R32_UINT,					42,  4, 1, 0 },
			{ VK_FORMAT_R32_SINT,					43,  4, 1, 0 },
			{ VK_FORMAT_D24_UNORM_S8_UINT,			45,  4, 1, e_Depth | e_Stencil },
			{ VK_FORMAT_R8G8_UNORM,					49,  2, 1, 0 },
			{ VK_FORMAT_R8G8_UINT,					50,  2, 1, 0 },
			{ VK_FORMAT_R8G8_SNORM,					51,  2, 1, 0 },
			{ VK_FORMAT_R8G8_SINT,					52,  2, 1, 0 },
			{ VK_FORMAT_R16_SFLOAT,					54,  2, 1, 0 },
			{ VK_FORMAT_D16_UNORM,					55,  2, 1, e_Depth },
			{ VK_FORMAT_R16_UNORM,					56,  2, 1, 0 },
			{ VK_FORMAT_R16_UINT,					57,  2, 1, 0 },
			{ VK_FORMAT_R16_SNORM,					58,  2, 1, 0 },
			{ VK_FORMAT_R16_SINT,					59,  2, 1, 0 },
			{ VK_FORMAT_R8_UNORM,					61,  1, 1, 0 },
			{ VK_FORMAT_R8_UINT,					62,  1, 1, 0 },
			{ VK_FORMAT_R8_SNORM,					63,  1, 1, 0 },
			{ VK_FORMAT_R8_SINT,					64,  1, 1, 0 },
			{ VK_FORMAT_A8_UNORM,					65,  1, 1, 0 },
			{ VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,		67,  4, 1, 0 },
			{ VK_FORMAT_BC1_RGBA_UNORM_BLOCK,		71,  8, 4, e_Compressed },
			{ VK_FORMAT_BC1_RGBA_SRGB_BLOCK,		72,  8, 4, e_Compressed | e_Srgb },
			{ VK_FORMAT_BC2_UNORM_BLOCK,			74,  16, 4, e_Compressed },
			{ VK_FORMAT_BC2_SRGB_BLOCK,				75,  16, 4, e_Compressed | e_Srgb },
			{ VK_FORMAT_BC3_UNORM_BLOCK,			77,  16, 4, e_Compressed },
			{ VK_FORMAT_BC3_SRGB_BLOCK,				78,  16, 4, e_Compressed | e_Srgb },
			{ VK_FORMAT_BC4_UNORM_BLOCK,			80,  8, 4, e_Compressed },
			{ VK_FORMAT_BC4_SNORM_BLOCK,			81,  8, 4, e_Compressed },
			{ VK_FORMAT_BC5_UNORM_BLOCK,			83,  16, 4, e_Compressed },
			{ VK_FORMAT_BC5_SNORM_BLOCK,			84,  16, 4, e_Compressed },
			//	DXGI's B5G6R5, B5G5R5A1 and B4G4R4A4 name the components from the
			//	least significant bit; Vulkan's PACK16 names from the most: the
			//	same bit layouts
			{ VK_FORMAT_R5G6B5_UNORM_PACK16,		85,  2, 1, 0 },
			{ VK_FORMAT_A1R5G5B5_UNORM_PACK16,		86,  2, 1, 0 },
			{ VK_FORMAT_B8G8R8A8_UNORM,				87,  4, 1, 0 },
			{ VK_FORMAT_B8G8R8A8_SRGB,				91,  4, 1, e_Srgb },
			{ VK_FORMAT_BC6H_UFLOAT_BLOCK,			95,  16, 4, e_Compressed },
			{ VK_FORMAT_BC6H_SFLOAT_BLOCK,			96,  16, 4, e_Compressed },
			{ VK_FORMAT_BC7_UNORM_BLOCK,			98,  16, 4, e_Compressed },
			{ VK_FORMAT_BC7_SRGB_BLOCK,				99,  16, 4, e_Compressed | e_Srgb },
			{ VK_FORMAT_A4R4G4B4_UNORM_PACK16,		115, 2, 1, 0 },
			//	Vulkan formats with no DXGI equivalent the port still uses
			{ VK_FORMAT_S8_UINT,					0,   1, 1, e_Stencil },
		};

		//	DXGI formats Vulkan lacks, given a substitute
		struct Substitution
		{
			uint32_t	m_Dxgi;
			VkFormat	m_Vk;
			const char*	m_pReason;
		};

		const Substitution c_Substitutions[] =
		{
			{ 88, VK_FORMAT_B8G8R8A8_UNORM,
			  "B8G8R8X8_UNORM: Vulkan has no X8 formats; B8G8R8A8 with alpha 255 (filled by the loader) reads the same" },
			{ 93, VK_FORMAT_B8G8R8A8_SRGB,
			  "B8G8R8X8_UNORM_SRGB: Vulkan has no X8 formats; B8G8R8A8_SRGB with alpha 255 (filled by the loader) reads the same" },
		};

		const Format* find(VkFormat i_Format)
		{
			for (const Format& f : c_Formats)
			{
				if (f.m_Vk == i_Format)
				{
					return &f;
				}
			}
			return nullptr;
		}
	}

	VkFormat vkdFormatFromDxgi(uint32_t i_Dxgi, const char** o_pSubstitution)
	{
		if (o_pSubstitution)
		{
			*o_pSubstitution = nullptr;
		}
		if (i_Dxgi == 0)
		{
			return VK_FORMAT_UNDEFINED;
		}
		for (const Format& f : c_Formats)
		{
			if (f.m_Dxgi == i_Dxgi)
			{
				return f.m_Vk;
			}
		}
		for (const Substitution& s : c_Substitutions)
		{
			if (s.m_Dxgi == i_Dxgi)
			{
				if (o_pSubstitution)
				{
					*o_pSubstitution = s.m_pReason;
				}
				return s.m_Vk;
			}
		}
		return VK_FORMAT_UNDEFINED;
	}

	uint32_t vkdDxgiFromFormat(VkFormat i_Format)
	{
		const Format* f = find(i_Format);
		return f ? f->m_Dxgi : 0;
	}

	vkdFormatInfo vkdFormatInfoOf(VkFormat i_Format)
	{
		vkdFormatInfo info;
		if (const Format* f = find(i_Format))
		{
			info.m_BlockBytes = f->m_Bytes;
			info.m_BlockWidth = info.m_BlockHeight = f->m_Block;
			info.m_bDepth = (f->m_Flags & e_Depth) != 0;
			info.m_bStencil = (f->m_Flags & e_Stencil) != 0;
			info.m_bCompressed = (f->m_Flags & e_Compressed) != 0;
			info.m_bSrgb = (f->m_Flags & e_Srgb) != 0;
		}
		return info;
	}

	VkImageAspectFlags vkdFormatAspects(VkFormat i_Format)
	{
		const vkdFormatInfo info = vkdFormatInfoOf(i_Format);
		VkImageAspectFlags aspects = 0;
		if (info.m_bDepth)
		{
			aspects |= VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		if (info.m_bStencil)
		{
			aspects |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		return aspects ? aspects : (VkImageAspectFlags)VK_IMAGE_ASPECT_COLOR_BIT;
	}
}
