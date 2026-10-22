/*****************************************************************************
**  vkdValidation.cpp
**
**      See vkdValidation.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDVALIDATION_HPP
#include "GraphicsVulkan/vkd/vkdValidation.hpp"
#endif
#include <cstring>

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gvk
{
	vkdValidationStats vkdValidation::Stats() const
	{
		vkdValidationStats stats;
		stats.m_Errors = m_Errors.load();
		stats.m_Warnings = m_Warnings.load();
		stats.m_Infos = m_Infos.load();
		stats.m_BestPractices = m_BestPractices.load();
		return stats;
	}

	void vkdValidation::Reset()
	{
		m_Errors = 0;
		m_Warnings = 0;
		m_Infos = 0;
		m_BestPractices = 0;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL vkdValidation::Callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT i_Severity,
		VkDebugUtilsMessageTypeFlagsEXT i_Types,
		const VkDebugUtilsMessengerCallbackDataEXT* i_pData,
		void* i_pUserData)
	{
		vkdValidation* self = static_cast<vkdValidation*>(i_pUserData);
		const char* id = (i_pData && i_pData->pMessageIdName) ? i_pData->pMessageIdName : "";
		const char* text = (i_pData && i_pData->pMessage) ? i_pData->pMessage : "";

		//	Only the validation layer's findings (validation and performance
		//	messages) count. General messages are the loader and layers
		//	talking about themselves (e.g. an implicit layer someone installed
		//	targets an older Vulkan): logged, counted as information.
		const VkDebugUtilsMessageTypeFlagsEXT c_Findings =
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		//	Suppressed (counted as information): the validation layer
		//	describing its own configuration when GPU-assisted validation is
		//	on - not findings about the application.
		//	  VALIDATION-SETTINGS: "GPU-AV and core checks both enabled, slow"
		//	  WARNING-Setting-Limit-Adjusted: GPU-AV forces features it needs
		//	    for its instrumentation (int64, buffer stores, ...)
		//	  WARNING-GPU-Assisted-Validation: GPU-AV enlarges the descriptor
		//	    heap's reserved range for itself ("Internal Warning")
		static const char* const c_LayerNotices[] =
		{
			"VALIDATION-SETTINGS",
			"WARNING-Setting-Limit-Adjusted",
			"WARNING-GPU-Assisted-Validation",
		};
		bool notice = false;
		for (const char* notice_id : c_LayerNotices)
		{
			notice = notice || strcmp(id, notice_id) == 0;
		}

		//	Best-practices advice (Khronos' and NVIDIA's): counted apart
		static const char c_BestPractices[] = "BestPractices-";
		if (strncmp(id, c_BestPractices, sizeof(c_BestPractices) - 1) == 0)
		{
			++self->m_BestPractices;
			DBG_LOG("Vulkan validation [best-practices] " << id << " | " << text);
			return VK_FALSE;
		}

		if (!(i_Types & c_Findings) || notice)
		{
			++self->m_Infos;
			if (i_Severity & (VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT))
			{
				DBG_LOG("Vulkan message [" << id << "] " << text);
			}
			return VK_FALSE;
		}

		if (i_Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			++self->m_Errors;
			DBG_LOG("Vulkan validation [error] " << id << " | " << text);
			DBG_ASSERT(false, "Vulkan validation [error] " << id << " | " << text);
		}
		else if (i_Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			++self->m_Warnings;
			DBG_LOG("Vulkan validation [warning] " << id << " | " << text);
		}
		else
		{
			++self->m_Infos;
		}
		return VK_FALSE;		// never abort the call
	}
}
