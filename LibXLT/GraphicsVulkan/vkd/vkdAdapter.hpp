/*****************************************************************************
**  vkdAdapter.hpp
**
**      Choosing the Vulkan device, on plain data so it can be tested
**	without a GPU (vkdDevice fills vkdAdapterInfo from the driver).
**
**	Selection uses Vulkan alone (no platform API): among the devices with
**	nothing missing, a discrete GPU first, then the most device-local
**	memory, then enumeration order. An override (MSP_GFX_VK_ADAPTER, a
**	case-insensitive name substring) picks a device by name instead.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDADAPTER_HPP
#error vkdAdapter.hpp multiply included
#endif
#define GVK_VKDADAPTER_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace gvk
{
	//	One physical device, as plain data.
	struct vkdAdapterInfo
	{
		std::string					m_Name;
		bool						m_bDiscrete = false;		// VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		uint64_t					m_DeviceLocalBytes = 0;		// sum of DEVICE_LOCAL heap sizes
		uint32_t					m_ApiVersion = 0;			// VkPhysicalDeviceProperties::apiVersion
		std::vector<std::string>	m_Extensions;				// supported device extensions
		std::vector<std::string>	m_Features;					// supported features among vkdRequiredFeatures()
	};

	//	The device extensions and features GraphicsVulkan needs (besides Vulkan 1.4).
	const std::vector<std::string>& vkdRequiredExtensions();
	const std::vector<std::string>& vkdRequiredFeatures();

	//	What i_Adapter lacks: "Vulkan 1.4" if its API version is older, then each
	//	missing extension and feature by name. Empty if it has everything.
	std::vector<std::string> vkdMissingRequirements(const vkdAdapterInfo& i_Adapter);

	//	The index of the adapter to use, or -1 with o_Error naming every adapter
	//	seen and what each lacks. i_Override (may be empty) is a case-insensitive
	//	name substring: the first adapter whose name contains it is used if it
	//	has everything; otherwise that is an error.
	int vkdSelectAdapter(const std::vector<vkdAdapterInfo>& i_Adapters,
						 const std::string& i_Override, std::string& o_Error);
}
