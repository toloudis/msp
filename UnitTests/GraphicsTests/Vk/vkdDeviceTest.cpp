/*****************************************************************************
**  vkdDeviceTest.cpp
**
**      vkdDevice on the real GPU: device choice, a clear read back, the
**      validation policy, deferred destruction, two threads submitting,
**      and a clean shutdown. Skipped without a Vulkan device.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtVulkan.hpp"

#ifndef GVK_VKDINFO_HPP
#include "GraphicsVulkan/vkd/vkdInfo.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <atomic>
#include <cstdlib>
#include <thread>

using namespace gvk;

namespace
{
	void clear(vkdDevice& i_Device, vkdImage& i_Image, float r, float g, float b, float a)
	{
		VkClearColorValue color = {};
		color.float32[0] = r; color.float32[1] = g; color.float32[2] = b; color.float32[3] = a;
		VkImageSubresourceRange range = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
		vkdFullBarrier(i_Device.Commands());	// order after earlier writes (sync validation)
		vkCmdClearColorImage(i_Device.Commands(), i_Image.Image(), VK_IMAGE_LAYOUT_GENERAL, &color, 1, &range);
	}

	std::atomic<int> l_Asserts{0};
	void count_assert(const std::string&) { ++l_Asserts; }
}

TEST_F(gtVulkan, CreatesOnTheBestAdapter)
{
	EXPECT_TRUE(vkdMissingRequirements(Device().Adapter()).empty());
	EXPECT_GE(Device().Properties().apiVersion, VK_API_VERSION_1_4);
	std::cout << "Vulkan device: " << Device().Adapter().m_Name
			  << (Device().Adapter().m_bDiscrete ? " (discrete)" : "") << std::endl;
}

TEST_F(gtVulkan, ClearAndReadBack)
{
	std::unique_ptr<vkdImage> image = vkdImage::Create2D(Device(), 64, 64, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, "ClearAndReadBack");
	clear(Device(), *image, 0.25f, 0.5f, 0.75f, 1.0f);
	std::vector<uint8_t> pixels = image->ReadBack();
	ASSERT_EQ(64u * 64u * 4u, pixels.size());
	for (size_t i = 0; i < pixels.size(); i += 4)
	{
		ASSERT_NEAR(64, pixels[i + 0], 1) << "pixel " << i / 4;
		ASSERT_NEAR(128, pixels[i + 1], 1);
		ASSERT_NEAR(191, pixels[i + 2], 1);
		ASSERT_EQ(255, pixels[i + 3]);
	}
}

TEST_F(gtVulkan, ValidationErrorIsCaught)
{
	l_Asserts = 0;
	dbgMsg::AssertHandler previous = dbgMsg::SetAssertHandler(&count_assert);
	//	Depth bounds outside 0..1 are invalid (no VK_EXT_depth_range_unrestricted)
	vkCmdSetDepthBounds(Device().Commands(), -1.0f, 1.0f);
	dbgMsg::SetAssertHandler(previous);

	//	DBG_ASSERT is compiled out of Release builds; the error is still counted and logged
	EXPECT_EQ(ENV_BUILD == ENV_DEBUGBUILD ? 1 : 0, l_Asserts.load());
	EXPECT_EQ(1, m_Validation.Stats().m_Errors);
	EXPECT_EQ(1, Device().ValidationStats().m_Errors);
	Device().Submit();
	Device().WaitIdle();
	m_Validation.Reset();		// the expected error must not fail TearDown
}

TEST_F(gtVulkan, DeferredDestroyRunsAfterGpu)
{
	std::unique_ptr<vkdImage> image = vkdImage::Create2D(Device(), 2048, 2048, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT, "DeferredDestroy");
	clear(Device(), *image, 1, 0, 0, 1);
	bool destroyed = false;
	Device().DeferDestroy([&destroyed]() { destroyed = true; });
	EXPECT_FALSE(destroyed);
	uint64_t value = Device().Submit();
	Device().WaitFor(value);
	EXPECT_TRUE(destroyed);
}

TEST_F(gtVulkan, SubmitFromTwoThreads)
{
	std::unique_ptr<vkdImage> a = vkdImage::Create2D(Device(), 256, 256, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT, "ThreadA");
	std::unique_ptr<vkdImage> b = vkdImage::Create2D(Device(), 256, 256, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT, "ThreadB");
	Device().Submit();

	auto work = [this](vkdImage* i_Image)
	{
		for (int i = 0; i < 100; ++i)
		{
			std::lock_guard<std::recursive_mutex> lock(Device().StreamLock());
			clear(Device(), *i_Image, i / 100.0f, 0, 0, 1);
			Device().Submit();
		}
	};
	std::thread t1(work, a.get());
	std::thread t2(work, b.get());
	t1.join();
	t2.join();
	Device().WaitIdle();
	EXPECT_GE(Device().CompletedValue(), 201u);
}

TEST(vkdDeviceLifetime, DestroyIsValidationClean)
{
	vkdValidation validation;
	vkdDeviceOptions options;
	options.m_bValidation = true;
	options.m_pValidation = &validation;
	std::string error;
	std::unique_ptr<vkdDevice> device(vkdDevice::Create(options, error));
	if (!device)
	{
		GTEST_SKIP() << "No usable Vulkan device: " << error;
	}
	std::unique_ptr<vkdImage> image = vkdImage::Create2D(*device, 128, 128, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT, "AliveAtShutdown");
	clear(*device, *image, 0, 1, 0, 1);
	image.reset();				// destruction deferred; the device must still run it
	device.reset();
	EXPECT_EQ(0, validation.Stats().m_Errors);
	EXPECT_EQ(0, validation.Stats().m_Warnings);
}

TEST(vkdDeviceLifetime, NoMatchingAdapterIsAReason)
{
	vkdDeviceOptions options;
	options.m_AdapterOverride = "no-such-gpu-anywhere";
	std::string error;
	std::unique_ptr<vkdDevice> device(vkdDevice::Create(options, error));
	EXPECT_FALSE(device);
	EXPECT_FALSE(error.empty());
}
