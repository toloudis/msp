/*****************************************************************************
**  vkdCheck.cpp
**
**      See vkdCheck.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif

#include <mutex>
#include <set>
#include <string>

namespace gvk
{
	const char* vkdResultName(VkResult i_Result)
	{
		switch (i_Result)
		{
#define GVK_RESULT_NAME(r) case r: return #r;
			GVK_RESULT_NAME(VK_SUCCESS)
			GVK_RESULT_NAME(VK_NOT_READY)
			GVK_RESULT_NAME(VK_TIMEOUT)
			GVK_RESULT_NAME(VK_EVENT_SET)
			GVK_RESULT_NAME(VK_EVENT_RESET)
			GVK_RESULT_NAME(VK_INCOMPLETE)
			GVK_RESULT_NAME(VK_SUBOPTIMAL_KHR)
			GVK_RESULT_NAME(VK_ERROR_OUT_OF_HOST_MEMORY)
			GVK_RESULT_NAME(VK_ERROR_OUT_OF_DEVICE_MEMORY)
			GVK_RESULT_NAME(VK_ERROR_INITIALIZATION_FAILED)
			GVK_RESULT_NAME(VK_ERROR_DEVICE_LOST)
			GVK_RESULT_NAME(VK_ERROR_MEMORY_MAP_FAILED)
			GVK_RESULT_NAME(VK_ERROR_LAYER_NOT_PRESENT)
			GVK_RESULT_NAME(VK_ERROR_EXTENSION_NOT_PRESENT)
			GVK_RESULT_NAME(VK_ERROR_FEATURE_NOT_PRESENT)
			GVK_RESULT_NAME(VK_ERROR_INCOMPATIBLE_DRIVER)
			GVK_RESULT_NAME(VK_ERROR_TOO_MANY_OBJECTS)
			GVK_RESULT_NAME(VK_ERROR_FORMAT_NOT_SUPPORTED)
			GVK_RESULT_NAME(VK_ERROR_FRAGMENTED_POOL)
			GVK_RESULT_NAME(VK_ERROR_UNKNOWN)
			GVK_RESULT_NAME(VK_ERROR_OUT_OF_POOL_MEMORY)
			GVK_RESULT_NAME(VK_ERROR_SURFACE_LOST_KHR)
			GVK_RESULT_NAME(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)
			GVK_RESULT_NAME(VK_ERROR_OUT_OF_DATE_KHR)
			GVK_RESULT_NAME(VK_ERROR_VALIDATION_FAILED_EXT)
#undef GVK_RESULT_NAME
			default: return "VK_RESULT_(unknown)";
		}
	}

	void vkdNotYet(const char* i_Name)
	{
		static std::mutex s_Lock;
		static std::set<std::string> s_Seen;
		std::lock_guard<std::mutex> lock(s_Lock);
		if (s_Seen.insert(i_Name).second)
		{
			DBG_LOG("GVK_NOT_YET: " << i_Name);
		}
	}
}
