/*****************************************************************************
**  vkdValidationStats.hpp
**
**      Counts of Vulkan validation messages by severity (see
**	vkdValidation.hpp). Plain data with no Vulkan headers, so code outside
**	GraphicsVulkan (MainApp) can read them through GraphicsVulkanLayer.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDVALIDATIONSTATS_HPP
#error vkdValidationStats.hpp multiply included
#endif
#define GVK_VKDVALIDATIONSTATS_HPP

namespace gvk
{
	struct vkdValidationStats
	{
		int m_Errors = 0;
		int m_Warnings = 0;
		int m_Infos = 0;
		int m_BestPractices = 0;		// MSP_GFX_VK_VALIDATION=bestpractices advice; fails nothing
	};
}
