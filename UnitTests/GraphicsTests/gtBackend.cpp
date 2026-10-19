/*****************************************************************************
**  gtBackend.cpp
**
**      See gtBackend.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtPlatform.hpp"

#ifndef GRAPHICS_LAYER_HPP
#include "Graphics/GraphicsLayer.hpp"
#endif
#ifndef GRAPHICSDX11_LAYER_HPP
#include "GraphicsDX11/GraphicsDX11Layer.hpp"
#endif
#ifndef G2D_DX11GLOBALWIN_HPP
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#endif
#ifndef GVK_GRAPHICSVULKANLAYER_HPP
#include "GraphicsVulkan/GraphicsVulkanLayer.hpp"
#endif
#ifndef GVK_VKDVALIDATIONSTATS_HPP
#include "GraphicsVulkan/vkd/vkdValidationStats.hpp"
#endif
#ifndef G3D_GFXOPTIONS_HPP
#include "Graphics/G3d/g3dGfxOptions.hpp"
#endif
#ifndef ENV_EXCEPTIONX_HPP
#include "Core/env/envExceptionX.hpp"
#endif
#include "Core/dbg/dbgMsg.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <string>

namespace
{
	//	GTEST_SKIP's effect from a function that returns a value
	void skip(const std::string& i_Why)
	{
		::testing::internal::AssertHelper(::testing::TestPartResult::kSkip, __FILE__, __LINE__, i_Why.c_str()) = ::testing::Message();
	}

	//	GraphicsDX11 is the reference, not what is tested: its own asserts
	//	(it asserts on inputs it then handles, e.g. BitsPerPixel of
	//	B4G4R4A4) are logged; any other assert fails the test as usual
	dbgMsg::AssertHandler l_PreviousHandler = NULL;
	void report_dx11_assert(const std::string& i_Message)
	{
		if (i_Message.find("\\GraphicsDX11\\") != std::string::npos)
		{
			std::cout << "  (GraphicsDX11 assert, not a failure) " << i_Message << std::endl;
		}
		else if (l_PreviousHandler)
		{
			l_PreviousHandler(i_Message);
		}
	}

	bool run_dx11(void* i_pWindow, const std::function<void()>& i_Function)
	{
		l_PreviousHandler = dbgMsg::SetAssertHandler(&report_dx11_assert);
		try
		{
			GraphicsDX11Layer::Init(i_pWindow);
		}
		catch (const envExceptionX& i_Ex)
		{
			dbgMsg::SetAssertHandler(l_PreviousHandler);
			skip("No Direct3D 11 device: " + i_Ex.GetErrorMessage());
			return false;
		}
		GraphicsLayer::InitGraphics(GraphicsDX11Layer::GetSystem2D(), GraphicsDX11Layer::GetSystem3D());
		GraphicsDX11Layer::InitGraphics();

		i_Function();

		//	g2dDX11Global keeps the current targets across devices (MachStudio
		//	makes one device per process); the next run mustn't inherit them
		g2dDX11Global::SetRenderTargets(NULL, NULL);
		GraphicsDX11Layer::CleanUpGraphics();
		GraphicsLayer::CleanUpGraphics();
		GraphicsDX11Layer::CleanUp();
		dbgMsg::SetAssertHandler(l_PreviousHandler);
		return true;
	}

	bool run_vulkan(void* i_pWindow, const std::function<void()>& i_Function)
	{
		g3dGfxOptions options;
		options.m_Backend = g3dGfxOptions::e_Vulkan;
		options.m_bValidation = true;
		if (!GraphicsVulkanLayer::Init(i_pWindow, options))
		{
			skip("No usable Vulkan device: " + GraphicsVulkanLayer::GetInitError());
			return false;
		}
		GraphicsLayer::InitGraphics(GraphicsVulkanLayer::GetSystem2D(), GraphicsVulkanLayer::GetSystem3D());
		GraphicsVulkanLayer::InitGraphics();

		i_Function();

		GraphicsVulkanLayer::CleanUpGraphics();
		GraphicsLayer::CleanUpGraphics();
		GraphicsVulkanLayer::CleanUp();

		const gvk::vkdValidationStats stats = GraphicsVulkanLayer::GetValidationStats();
		EXPECT_EQ(0, stats.m_Errors) << "Vulkan validation errors (see the log)";
		EXPECT_EQ(0, stats.m_Warnings) << "Vulkan validation warnings (see the log)";
		return true;
	}
}

const char* gtBackend::Name(Kind i_Kind)
{
	return i_Kind == e_DX11 ? "DX11" : "Vulkan";
}

bool gtBackend::Run(Kind i_Kind, const std::function<void()>& i_Function)
{
	SCOPED_TRACE(Name(i_Kind));
	GraphicsLayer::Init();
	void* window = gtPlatform::CreateHiddenWindow(64, 64);
	const bool ran = (i_Kind == e_DX11) ? run_dx11(window, i_Function) : run_vulkan(window, i_Function);
	gtPlatform::DestroyHiddenWindow(window);
	GraphicsLayer::CleanUp();
	return ran;
}
