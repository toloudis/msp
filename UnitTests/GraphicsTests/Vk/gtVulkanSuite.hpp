/*****************************************************************************
**  gtVulkanSuite.hpp
**
**      gtVulkanSuite is gtVulkanContext made once for a whole test suite
**      instead of once per test: one device (validation on), bindings,
**      ring buffer, pipeline cache and vkdContext shared by the suite's
**      tests, for parameterized suites that run many small tests (a new
**      device per test would cost most of their time). Each test still
**      fails on any validation message it caused (the count is reset in
**      SetUp and checked in TearDown), and the suite fails on any reported
**      while the device is destroyed. Images a test makes are released
**      when it ends. Skips every test when there is no usable Vulkan
**      device.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include "Vk/gtVulkanContext.hpp"

#include <gtest/gtest.h>

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

class gtVulkanSuite : public ::testing::Test
{
	public:
		static void SetUpTestSuite()
		{
			Shared& shared = Get();
			gvk::vkdDeviceOptions options;
			options.m_bValidation = true;
			options.m_pValidation = &shared.m_Validation;
			if (const char* adapter = std::getenv("MSP_GFX_VK_ADAPTER"))
			{
				options.m_AdapterOverride = adapter;
			}
			shared.m_pDevice.reset(gvk::vkdDevice::Create(options, shared.m_Error));
			if (!shared.m_pDevice)
			{
				return;
			}
			gvk::vkdDevice& device = *shared.m_pDevice;
			shared.m_pBindings.reset(new gvk::vkdBindings(device));
			shared.m_pRing.reset(new gvk::vkdRingBuffer(device, 4 << 20));
			shared.m_pPipelines.reset(new gvk::vkdPipelineCache(device, *shared.m_pBindings, ""));
			shared.m_pContext.reset(new gvk::vkdContext(device, *shared.m_pRing, *shared.m_pBindings, *shared.m_pPipelines));
		}

		static void TearDownTestSuite()
		{
			Shared& shared = Get();
			if (shared.m_pDevice)
			{
				shared.m_Validation.Reset();
				shared.m_pContext.reset();
				shared.m_pPipelines.reset();
				shared.m_pRing.reset();
				shared.m_pBindings.reset();
				shared.m_pDevice.reset();
				const gvk::vkdValidationStats stats = shared.m_Validation.Stats();
				EXPECT_EQ(0, stats.m_Errors) << "Vulkan validation errors destroying the device (see the log)";
				EXPECT_EQ(0, stats.m_Warnings) << "Vulkan validation warnings destroying the device (see the log)";
			}
			shared.m_Error.clear();
		}

	protected:
		void SetUp() override
		{
			Shared& shared = Get();
			if (!shared.m_pDevice)
			{
				GTEST_SKIP() << "No usable Vulkan device: " << shared.m_Error;
			}
			shared.m_Validation.Reset();
			gvk::vkdGlobal::Set(shared.m_pDevice.get(), shared.m_pRing.get(), shared.m_pBindings.get(),
								shared.m_pPipelines.get(), shared.m_pContext.get());
		}

		//	Subclasses release what they made from the context before this runs
		void TearDown() override
		{
			Shared& shared = Get();
			if (!shared.m_pDevice)
			{
				return;
			}
			shared.m_pContext->Flush();
			shared.m_pDevice->WaitIdle();
			m_Images.clear();
			gvk::vkdGlobal::Set(NULL, NULL, NULL, NULL, NULL);
			const gvk::vkdValidationStats stats = shared.m_Validation.Stats();
			EXPECT_EQ(0, stats.m_Errors) << "Vulkan validation errors (see the log)";
			EXPECT_EQ(0, stats.m_Warnings) << "Vulkan validation warnings (see the log)";
		}

		bool HasDevice() const { return Get().m_pDevice != nullptr; }
		gvk::vkdDevice& Device() { return *Get().m_pDevice; }
		gvk::vkdContext& Context() { return *Get().m_pContext; }

		//	An R8G8B8A8 render target that can also be sampled, cleared and read back
		gvk::vkdImage* Target(uint32_t i_Width, uint32_t i_Height)
		{
			m_Images.push_back(gtVulkanImages::Target(Device(), i_Width, i_Height));
			return m_Images.back().get();
		}

		//	An R8G8B8A8 texture holding i_pRgba (rows tightly packed)
		gvk::vkdImage* Texture(uint32_t i_Width, uint32_t i_Height, const uint8_t* i_pRgba)
		{
			m_Images.push_back(gtVulkanImages::Texture(Device(), Context(), i_Width, i_Height, i_pRgba));
			return m_Images.back().get();
		}

		std::vector<uint8_t> Read(gvk::vkdImage* i_pImage)
		{
			return gtVulkanImages::Read(Context(), i_pImage);
		}

	private:
		struct Shared
		{
			gvk::vkdValidation						m_Validation;
			std::unique_ptr<gvk::vkdDevice>			m_pDevice;
			std::unique_ptr<gvk::vkdBindings>		m_pBindings;
			std::unique_ptr<gvk::vkdRingBuffer>		m_pRing;
			std::unique_ptr<gvk::vkdPipelineCache>	m_pPipelines;
			std::unique_ptr<gvk::vkdContext>		m_pContext;
			std::string								m_Error;
		};

		//	One at a time: gtest runs suites one after the other
		static Shared& Get()
		{
			static Shared s_Shared;
			return s_Shared;
		}

		std::vector<std::unique_ptr<gvk::vkdImage>> m_Images;
};
