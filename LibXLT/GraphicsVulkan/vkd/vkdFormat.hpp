/*****************************************************************************
**  vkdFormat.hpp
**
**      DXGI formats (as plain uint32_t DXGI_FORMAT values: GraphicsVulkan
**	doesn't include dxgiformat.h) to Vulkan formats and back, and the block
**	size of each Vulkan format the port uses.
**
**	Where Vulkan has no equivalent, vkdFormatFromDxgi either returns
**	VK_FORMAT_UNDEFINED (unsupported) or substitutes a format and says why
**	in o_pSubstitution. The substitutions are in the table in vkdFormat.cpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDFORMAT_HPP
#error vkdFormat.hpp multiply included
#endif
#define GVK_VKDFORMAT_HPP

#include <volk.h>

#include <cstdint>

namespace gvk
{
	//	VK_FORMAT_UNDEFINED if unsupported. o_pSubstitution (optional) is set to
	//	NULL for an exact equivalent, or to the reason for a substitution.
	VkFormat vkdFormatFromDxgi(uint32_t i_Dxgi, const char** o_pSubstitution = nullptr);

	//	0 (DXGI_FORMAT_UNKNOWN) if the format has no DXGI equivalent
	uint32_t vkdDxgiFromFormat(VkFormat i_Format);

	struct vkdFormatInfo
	{
		uint32_t	m_BlockBytes = 0;		// bytes per pixel, or per block for compressed formats; 0 = unknown format
		uint32_t	m_BlockWidth = 1;
		uint32_t	m_BlockHeight = 1;
		bool		m_bDepth = false;
		bool		m_bStencil = false;
		bool		m_bCompressed = false;
		bool		m_bSrgb = false;
	};

	vkdFormatInfo vkdFormatInfoOf(VkFormat i_Format);

	//	The aspects of a format: colour, or its depth and stencil
	VkImageAspectFlags vkdFormatAspects(VkFormat i_Format);
}
