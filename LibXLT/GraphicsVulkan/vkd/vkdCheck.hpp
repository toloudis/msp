/*****************************************************************************
**  vkdCheck.hpp
**
**      Result checking and work-in-progress markers for GraphicsVulkan.
**
**	VK_CHECK(expr) evaluates a Vulkan call; if it fails, DBG_ASSERTs with
**	the expression and the result's name. It evaluates to the VkResult.
**
**	GVK_NOT_YET("Class::Method") marks a method that doesn't do its real
**	work yet (a stub that keeps the application running); the first time
**	each one runs it logs "GVK_NOT_YET: Class::Method". Phase 1 isn't done
**	while any are left.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDCHECK_HPP
#error vkdCheck.hpp multiply included
#endif
#define GVK_VKDCHECK_HPP

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <volk.h>

namespace gvk
{
	//	vkdResultName - "VK_ERROR_DEVICE_LOST" etc.
	const char* vkdResultName(VkResult i_Result);

	//	vkdNotYet - logs i_Name the first time it is called for that name
	void vkdNotYet(const char* i_Name);

	inline VkResult vkdCheck(VkResult i_Result, const char* i_Expression, const char* i_File, int i_Line)
	{
		if (i_Result < 0)
		{
			DBG_ASSERT(false, "Vulkan call failed: " << i_Expression << " -> " << vkdResultName(i_Result)
							  << " (" << i_File << ":" << i_Line << ")");
		}
		return i_Result;
	}
}

#define VK_CHECK(expr) ::gvk::vkdCheck((expr), #expr, __FILE__, __LINE__)
#define GVK_NOT_YET(name) ::gvk::vkdNotYet(name)
