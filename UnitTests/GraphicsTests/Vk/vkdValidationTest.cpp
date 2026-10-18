/*****************************************************************************
**  vkdValidationTest.cpp
**
**      The validation policy's counting, with synthetic messages (no GPU):
**      validation and performance messages count as errors/warnings;
**      general messages (the loader, e.g. about an implicit layer someone
**      installed) are information only.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDVALIDATION_HPP
#include "GraphicsVulkan/vkd/vkdValidation.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <gtest/gtest.h>

using namespace gvk;

namespace
{
	int l_Asserts = 0;
	void count_assert(const std::string&) { ++l_Asserts; }

	void send(vkdValidation& i_Validation, VkDebugUtilsMessageSeverityFlagBitsEXT i_Severity,
			  VkDebugUtilsMessageTypeFlagsEXT i_Types, const char* i_Id)
	{
		VkDebugUtilsMessengerCallbackDataEXT data = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CALLBACK_DATA_EXT};
		data.pMessageIdName = i_Id;
		data.pMessage = "test message";
		vkdValidation::Callback(i_Severity, i_Types, &data, &i_Validation);
	}
}

TEST(vkdValidation, ValidationErrorCountsAndAsserts)
{
	vkdValidation validation;
	l_Asserts = 0;
	dbgMsg::AssertHandler previous = dbgMsg::SetAssertHandler(&count_assert);
	send(validation, VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, "VUID-x");
	dbgMsg::SetAssertHandler(previous);
	EXPECT_EQ(1, validation.Stats().m_Errors);
	//	DBG_ASSERT is compiled out of Release builds; the error is still counted and logged
	EXPECT_EQ(ENV_BUILD == ENV_DEBUGBUILD ? 1 : 0, l_Asserts);
}

TEST(vkdValidation, ValidationAndPerformanceWarningsCount)
{
	vkdValidation validation;
	send(validation, VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, "w1");
	send(validation, VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT, "w2");
	EXPECT_EQ(2, validation.Stats().m_Warnings);
	EXPECT_EQ(0, validation.Stats().m_Errors);
}

TEST(vkdValidation, GeneralMessagesAreInformation)
{
	vkdValidation validation;
	send(validation, VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, "Loader Message");
	send(validation, VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, "Loader Message");
	EXPECT_EQ(0, validation.Stats().m_Warnings);
	EXPECT_EQ(0, validation.Stats().m_Errors);
	EXPECT_EQ(2, validation.Stats().m_Infos);
}

TEST(vkdValidation, ResetClears)
{
	vkdValidation validation;
	send(validation, VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, "w");
	validation.Reset();
	EXPECT_EQ(0, validation.Stats().m_Warnings);
}

TEST(vkdValidation, LayerSetupNoticesAreInformation)
{
	//	The validation layer describing its own configuration (GPU-assisted
	//	validation's setup) isn't a finding about the application.
	vkdValidation validation;
	for (const char* id : {"VALIDATION-SETTINGS", "WARNING-Setting-Limit-Adjusted", "WARNING-GPU-Assisted-Validation"})
	{
		send(validation, VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, id);
	}
	EXPECT_EQ(0, validation.Stats().m_Warnings);
	EXPECT_EQ(3, validation.Stats().m_Infos);
	//	other warnings still count
	send(validation, VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, "SYNC-HAZARD-WRITE-AFTER-WRITE");
	EXPECT_EQ(1, validation.Stats().m_Warnings);
}

TEST(vkdValidation, UnreadShaderOutputsAreInformation)
{
	//	A vertex shader writing an output the pixel shader doesn't read is
	//	valid in Vulkan and in D3D11, and the shaders are GraphicsDX11's,
	//	unchanged: logged, not counted as a warning.
	vkdValidation validation;
	send(validation, VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
		 "WARNING-Shader-OutputNotConsumed");
	EXPECT_EQ(0, validation.Stats().m_Warnings);
	EXPECT_EQ(1, validation.Stats().m_Infos);
	//	reading an input the previous stage doesn't write is still a finding
	send(validation, VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
		 "WARNING-Shader-InputNotProduced");
	EXPECT_EQ(1, validation.Stats().m_Warnings);
}
