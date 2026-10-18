/*****************************************************************************
**  gtVulkan.hpp
**
**      gtVulkan is the fixture for tests that need a Vulkan device: it
**      creates one with validation on (core and synchronization; GPU-
**      assisted too when the test asks for it), skips the test when there
**      is no usable Vulkan device (e.g. on CI), and fails the test if the
**      validation layer reported anything, including while the device was
**      being destroyed.
**
**      MSP_GFX_VK_ADAPTER picks the device by name, as in MachStudio.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif

#include <gtest/gtest.h>

#include <cstdlib>
#include <memory>
#include <string>

class gtVulkan : public ::testing::Test
{
	protected:
		void SetUp() override
		{
			gvk::vkdDeviceOptions options;
			options.m_bValidation = true;
			options.m_bGpuAssisted = UseGpuAssistedValidation();
			options.m_pValidation = &m_Validation;
			if (const char* adapter = std::getenv("MSP_GFX_VK_ADAPTER"))
			{
				options.m_AdapterOverride = adapter;
			}
			std::string error;
			m_pDevice.reset(gvk::vkdDevice::Create(options, error));
			if (!m_pDevice)
			{
				GTEST_SKIP() << "No usable Vulkan device: " << error;
			}
		}

		void TearDown() override
		{
			if (m_pDevice)
			{
				m_pDevice.reset();
				gvk::vkdValidationStats stats = m_Validation.Stats();
				EXPECT_EQ(0, stats.m_Errors) << "Vulkan validation errors (see the log)";
				EXPECT_EQ(0, stats.m_Warnings) << "Vulkan validation warnings (see the log)";
			}
		}

		virtual bool UseGpuAssistedValidation() const { return false; }

		gvk::vkdDevice& Device() { return *m_pDevice; }

		gvk::vkdValidation m_Validation;
		std::unique_ptr<gvk::vkdDevice> m_pDevice;
};
