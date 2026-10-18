/*****************************************************************************
**  vkdValidation.hpp
**
**      The Vulkan validation policy: messages from VK_LAYER_KHRONOS_validation
**	(through VK_EXT_debug_utils) are counted by severity; errors DBG_ASSERT
**	with the message ID and text, warnings are logged. A successful port
**	means zero errors and zero warnings. Lines read "Vulkan validation
**	[error] <ID> | <text>": not "error <ID>:", which MSBuild would take
**	for a build error when the tests run after linking.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDVALIDATION_HPP
#error vkdValidation.hpp multiply included
#endif
#define GVK_VKDVALIDATION_HPP

#include <volk.h>

#include <atomic>

namespace gvk
{
	struct vkdValidationStats
	{
		int m_Errors = 0;
		int m_Warnings = 0;
		int m_Infos = 0;
	};

	class vkdValidation
	{
		public:
			vkdValidationStats Stats() const;
			void Reset();

			//	The debug-utils messenger callback; i_pUserData is the vkdValidation.
			static VKAPI_ATTR VkBool32 VKAPI_CALL Callback(
				VkDebugUtilsMessageSeverityFlagBitsEXT i_Severity,
				VkDebugUtilsMessageTypeFlagsEXT i_Types,
				const VkDebugUtilsMessengerCallbackDataEXT* i_pData,
				void* i_pUserData);

		private:
			std::atomic<int> m_Errors{0};
			std::atomic<int> m_Warnings{0};
			std::atomic<int> m_Infos{0};
	};
}
