/*****************************************************************************
**  vkdAdapterTest.cpp
**
**      Vulkan device selection and the requirement check, on plain data
**      (no GPU needed).
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDADAPTER_HPP
#include "GraphicsVulkan/vkd/vkdAdapter.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

using namespace gvk;

namespace
{
	const uint32_t c_Vulkan13 = (1u << 22) | (3u << 12);
	const uint32_t c_Vulkan14 = (1u << 22) | (4u << 12);
	const uint64_t c_GB = 1024ull * 1024ull * 1024ull;

	vkdAdapterInfo complete(const char* i_Name, bool i_bDiscrete, uint64_t i_Bytes)
	{
		vkdAdapterInfo info;
		info.m_Name = i_Name;
		info.m_bDiscrete = i_bDiscrete;
		info.m_DeviceLocalBytes = i_Bytes;
		info.m_ApiVersion = c_Vulkan14;
		info.m_Extensions = vkdRequiredExtensions();
		info.m_Features = vkdRequiredFeatures();
		return info;
	}

	vkdAdapterInfo intel()  { return complete("Intel(R) UHD Graphics P630", false, 1 * c_GB); }
	vkdAdapterInfo quadro() { return complete("Quadro RTX 5000", true, 16 * c_GB); }

	void remove(std::vector<std::string>& io_List, const char* i_Name)
	{
		io_List.erase(std::remove(io_List.begin(), io_List.end(), std::string(i_Name)), io_List.end());
	}

	bool contains(const std::string& i_Text, const char* i_Part)
	{
		return i_Text.find(i_Part) != std::string::npos;
	}
}

TEST(vkdAdapter, PrefersDiscrete)
{
	std::string error;
	EXPECT_EQ(1, vkdSelectAdapter({intel(), quadro()}, "", error));
	EXPECT_EQ("", error);
}

TEST(vkdAdapter, MoreMemoryWins)
{
	std::string error;
	EXPECT_EQ(1, vkdSelectAdapter({complete("A", true, 8 * c_GB), complete("B", true, 16 * c_GB)}, "", error));
}

TEST(vkdAdapter, EqualCandidatesKeepEnumerationOrder)
{
	std::string error;
	EXPECT_EQ(0, vkdSelectAdapter({complete("A", true, 8 * c_GB), complete("B", true, 8 * c_GB)}, "", error));
}

TEST(vkdAdapter, SkipsDevicesMissingRequirements)
{
	vkdAdapterInfo old_discrete = quadro();
	old_discrete.m_ApiVersion = c_Vulkan13;
	std::string error;
	EXPECT_EQ(1, vkdSelectAdapter({old_discrete, intel()}, "", error));
}

TEST(vkdAdapter, NoSuitableDeviceIsAnError)
{
	vkdAdapterInfo a = intel();
	a.m_ApiVersion = c_Vulkan13;
	vkdAdapterInfo b = quadro();
	remove(b.m_Extensions, "VK_EXT_descriptor_heap");
	std::string error;
	EXPECT_EQ(-1, vkdSelectAdapter({a, b}, "", error));
	EXPECT_TRUE(contains(error, "Intel(R) UHD Graphics P630")) << error;
	EXPECT_TRUE(contains(error, "Vulkan 1.4")) << error;
	EXPECT_TRUE(contains(error, "Quadro RTX 5000")) << error;
	EXPECT_TRUE(contains(error, "VK_EXT_descriptor_heap")) << error;
}

TEST(vkdAdapter, NoDevicesIsAnError)
{
	std::string error;
	EXPECT_EQ(-1, vkdSelectAdapter({}, "", error));
	EXPECT_FALSE(error.empty());
}

TEST(vkdAdapter, OverrideSelectsByName)
{
	std::string error;
	EXPECT_EQ(0, vkdSelectAdapter({intel(), quadro()}, "uhd", error));
}

TEST(vkdAdapter, OverrideMissingRequirementsIsAnError)
{
	vkdAdapterInfo a = intel();
	a.m_ApiVersion = c_Vulkan13;
	std::string error;
	EXPECT_EQ(-1, vkdSelectAdapter({a, quadro()}, "UHD", error));
	EXPECT_TRUE(contains(error, "Vulkan 1.4")) << error;
}

TEST(vkdAdapter, OverrideMatchingNothingIsAnError)
{
	std::string error;
	EXPECT_EQ(-1, vkdSelectAdapter({intel(), quadro()}, "radeon", error));
	EXPECT_TRUE(contains(error, "radeon")) << error;
	EXPECT_TRUE(contains(error, "Quadro RTX 5000")) << error;
}

TEST(vkdAdapter, CompleteAdapterHasNothingMissing)
{
	EXPECT_TRUE(vkdMissingRequirements(quadro()).empty());
}

TEST(vkdAdapter, Vulkan13IsMissing14)
{
	vkdAdapterInfo a = quadro();
	a.m_ApiVersion = c_Vulkan13;
	EXPECT_EQ(std::vector<std::string>{"Vulkan 1.4"}, vkdMissingRequirements(a));
}

TEST(vkdAdapter, MissingExtensionNamed)
{
	vkdAdapterInfo a = quadro();
	remove(a.m_Extensions, "VK_EXT_descriptor_heap");
	EXPECT_EQ(std::vector<std::string>{"VK_EXT_descriptor_heap"}, vkdMissingRequirements(a));
}

TEST(vkdAdapter, MissingFeatureNamed)
{
	vkdAdapterInfo a = quadro();
	remove(a.m_Features, "tessellationShader");
	EXPECT_EQ(std::vector<std::string>{"tessellationShader"}, vkdMissingRequirements(a));
}

TEST(vkdAdapter, RequiredListsCoverThePlan)
{
	const std::vector<std::string>& ext = vkdRequiredExtensions();
	for (const char* name : {"VK_KHR_swapchain", "VK_EXT_descriptor_heap", "VK_KHR_unified_image_layouts",
							 "VK_KHR_calibrated_timestamps", "VK_EXT_memory_budget"})
	{
		EXPECT_NE(ext.end(), std::find(ext.begin(), ext.end(), std::string(name))) << name;
	}
	const std::vector<std::string>& feat = vkdRequiredFeatures();
	for (const char* name : {"geometryShader", "tessellationShader", "dynamicRendering", "synchronization2",
							 "timelineSemaphore", "descriptorHeap", "unifiedImageLayouts", "bufferDeviceAddress",
							 "bresenhamLines", "hostQueryReset", "occlusionQueryPrecise"})
	{
		EXPECT_NE(feat.end(), std::find(feat.begin(), feat.end(), std::string(name))) << name;
	}
}
