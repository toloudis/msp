/*****************************************************************************
**  vkdAdapter.hpp
**
**      Choosing the Vulkan device, on plain data so it can be tested
**	without a GPU (vkdDevice fills vkdAdapterInfo from the driver).
**
**	Selection uses Vulkan alone (no platform API): among the devices with
**	nothing missing, a discrete GPU first, then the most device-local
**	memory, then enumeration order. An override (MSP_GFX_VK_ADAPTER, a
**	case-insensitive name substring) picks a device by name instead. A
**	required device UUID (GraphicsOptiX: the CUDA device's, so both APIs
**	share one GPU) picks the device with that UUID and nothing else; the
**	override is then ignored.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDADAPTER_HPP
#error vkdAdapter.hpp multiply included
#endif
#define GVK_VKDADAPTER_HPP

#include <array>
#include <cstdint>
#include <optional>
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
		std::array<uint8_t, 16>		m_DeviceUUID = {};			// VkPhysicalDeviceIDProperties::deviceUUID
	};

	//	What a caller asks of the device besides what GraphicsVulkan needs.
	struct vkdAdapterRequest
	{
		std::string								m_Override;		// MSP_GFX_VK_ADAPTER; ignored with m_RequiredUUID
		std::optional<std::array<uint8_t, 16>>	m_RequiredUUID;	// only the device with this UUID
		std::string								m_UUIDOwner;	// who it is, for messages ("the CUDA device \"...\"")
		std::vector<std::string>				m_Extensions;	// device extensions needed besides vkdRequiredExtensions()
	};

	//	The device extensions and features GraphicsVulkan needs (besides Vulkan 1.4).
	const std::vector<std::string>& vkdRequiredExtensions();
	const std::vector<std::string>& vkdRequiredFeatures();

	//	What i_Adapter lacks: "Vulkan 1.4" if its API version is older, then each
	//	missing extension (vkdRequiredExtensions(), then i_ExtraExtensions) and
	//	feature by name. Empty if it has everything.
	std::vector<std::string> vkdMissingRequirements(const vkdAdapterInfo& i_Adapter,
													const std::vector<std::string>& i_ExtraExtensions = {});

	//	The index of the adapter to use, or -1 with o_Error naming every adapter
	//	seen and what each lacks. i_Override (may be empty) is a case-insensitive
	//	name substring: the first adapter whose name contains it is used if it
	//	has everything; otherwise that is an error.
	int vkdSelectAdapter(const std::vector<vkdAdapterInfo>& i_Adapters,
						 const std::string& i_Override, std::string& o_Error);

	//	The same with everything a caller can ask: with a required UUID, the
	//	adapter that has it, if it has everything (and the extra extensions),
	//	else -1 and o_Error says which device was wanted and what it lacks.
	int vkdSelectAdapter(const std::vector<vkdAdapterInfo>& i_Adapters,
						 const vkdAdapterRequest& i_Request, std::string& o_Error);
}
