/*****************************************************************************
**  vkdAdapter.cpp
**
**      See vkdAdapter.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDADAPTER_HPP
#include "GraphicsVulkan/vkd/vkdAdapter.hpp"
#endif

#include <algorithm>
#include <cctype>
#include <sstream>

namespace gvk
{
	namespace
	{
		const uint32_t c_Vulkan14 = (1u << 22) | (4u << 12);	// VK_API_VERSION_1_4 (variant 0)

		uint32_t major_minor(uint32_t i_Version)
		{
			return i_Version & ~0xFFFu;		// drop the patch number
		}

		std::string lower(std::string i_Text)
		{
			for (char& c : i_Text)
			{
				c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
			}
			return i_Text;
		}

		bool has(const std::vector<std::string>& i_List, const std::string& i_Name)
		{
			return std::find(i_List.begin(), i_List.end(), i_Name) != i_List.end();
		}

		std::string describe(const std::vector<vkdAdapterInfo>& i_Adapters)
		{
			if (i_Adapters.empty())
			{
				return "no Vulkan devices were found";
			}
			std::ostringstream text;
			for (size_t i = 0; i < i_Adapters.size(); ++i)
			{
				std::vector<std::string> missing = vkdMissingRequirements(i_Adapters[i]);
				text << (i ? "; " : "") << "\"" << i_Adapters[i].m_Name << "\"";
				if (missing.empty())
				{
					text << " (has everything)";
				}
				else
				{
					text << " lacks ";
					for (size_t m = 0; m < missing.size(); ++m)
					{
						text << (m ? ", " : "") << missing[m];
					}
				}
			}
			return text.str();
		}
	}

	const std::vector<std::string>& vkdRequiredExtensions()
	{
		static const std::vector<std::string> s_Extensions =
		{
			"VK_KHR_swapchain",
			"VK_EXT_descriptor_heap",
			"VK_KHR_unified_image_layouts",
			"VK_KHR_calibrated_timestamps",
			"VK_EXT_memory_budget",
		};
		return s_Extensions;
	}

	const std::vector<std::string>& vkdRequiredFeatures()
	{
		static const std::vector<std::string> s_Features =
		{
			"geometryShader",
			"tessellationShader",
			"samplerAnisotropy",
			"fillModeNonSolid",
			"depthClamp",
			"depthBiasClamp",
			"independentBlend",
			"dualSrcBlend",
			"textureCompressionBC",
			"imageCubeArray",
			"shaderImageGatherExtended",	// the shaders' SPIR-V capabilities (fxEffectVkCapabilities test)
			"shaderClipDistance",
			"fragmentStoresAndAtomics",		// pixel-shader UAVs (the hair opacity volume), as D3D11 has them
			"shaderDrawParameters",
			"shaderDemoteToHelperInvocation",
			"samplerMirrorClampToEdge",
			"scalarBlockLayout",			// constant buffers in D3D packing (DXC -fvk-use-dx-layout)
			"dynamicRendering",
			"synchronization2",
			"timelineSemaphore",
			"bufferDeviceAddress",
			"hostQueryReset",				// vkdOcclusionQuery resets its query on the host
			"occlusionQueryPrecise",		// ...and counts samples exactly, as D3D11 does
			"descriptorHeap",
			"unifiedImageLayouts",
			"bresenhamLines",				// D3D11's aliased lines (diamond exit)
			"rectangularLines",				// D3D11's multisample-enabled lines: quadrilaterals...
			"wideLines",					// ...1.4 pixels wide
		};
		return s_Features;
	}

	std::vector<std::string> vkdMissingRequirements(const vkdAdapterInfo& i_Adapter)
	{
		std::vector<std::string> missing;
		if (major_minor(i_Adapter.m_ApiVersion) < c_Vulkan14)
		{
			missing.push_back("Vulkan 1.4");
		}
		for (const std::string& name : vkdRequiredExtensions())
		{
			if (!has(i_Adapter.m_Extensions, name))
			{
				missing.push_back(name);
			}
		}
		for (const std::string& name : vkdRequiredFeatures())
		{
			if (!has(i_Adapter.m_Features, name))
			{
				missing.push_back(name);
			}
		}
		return missing;
	}

	int vkdSelectAdapter(const std::vector<vkdAdapterInfo>& i_Adapters,
						 const std::string& i_Override, std::string& o_Error)
	{
		o_Error.clear();
		if (!i_Override.empty())
		{
			const std::string wanted = lower(i_Override);
			for (size_t i = 0; i < i_Adapters.size(); ++i)
			{
				if (lower(i_Adapters[i].m_Name).find(wanted) != std::string::npos)
				{
					if (vkdMissingRequirements(i_Adapters[i]).empty())
					{
						return static_cast<int>(i);
					}
					o_Error = "The Vulkan device chosen with MSP_GFX_VK_ADAPTER=\"" + i_Override +
							  "\" can't be used: " + describe(std::vector<vkdAdapterInfo>(1, i_Adapters[i]));
					return -1;
				}
			}
			o_Error = "No Vulkan device matches MSP_GFX_VK_ADAPTER=\"" + i_Override + "\". Devices: " + describe(i_Adapters);
			return -1;
		}

		int best = -1;
		for (size_t i = 0; i < i_Adapters.size(); ++i)
		{
			const vkdAdapterInfo& candidate = i_Adapters[i];
			if (!vkdMissingRequirements(candidate).empty())
			{
				continue;
			}
			if (best < 0)
			{
				best = static_cast<int>(i);
				continue;
			}
			const vkdAdapterInfo& current = i_Adapters[best];
			if (candidate.m_bDiscrete != current.m_bDiscrete)
			{
				if (candidate.m_bDiscrete)
				{
					best = static_cast<int>(i);
				}
			}
			else if (candidate.m_DeviceLocalBytes > current.m_DeviceLocalBytes)
			{
				best = static_cast<int>(i);
			}
		}
		if (best < 0)
		{
			o_Error = "No Vulkan device has everything GraphicsVulkan needs. Devices: " + describe(i_Adapters);
		}
		return best;
	}
}
