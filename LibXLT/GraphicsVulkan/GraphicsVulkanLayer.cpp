/*****************************************************************************
**  GraphicsVulkanLayer.cpp
**
**      See GraphicsVulkanLayer.hpp. Hooks are installed where GraphicsDX11
**	installs its own: the 2D and 3D systems install theirs while they
**	exist; Init installs the fragment creators (as bumpPackage/mayPackage
**	Init do); InitGraphics installs the scene renderers and baking (as
**	shdwPackage::InitGraphics does).
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_GRAPHICSVULKANLAYER_HPP
#include "GraphicsVulkan/GraphicsVulkanLayer.hpp"
#endif
#ifndef GVK_VKDVALIDATION_HPP
#include "GraphicsVulkan/vkd/vkdValidation.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDRINGBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdRingBuffer.hpp"
#endif
#ifndef GVK_VKDBINDINGS_HPP
#include "GraphicsVulkan/vkd/vkdBindings.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_G2DSYSTEMVK_HPP
#include "GraphicsVulkan/G2d/g2dSystemVk.hpp"
#endif
#ifndef GVK_G3DSYSTEMVK_HPP
#include "GraphicsVulkan/G3d/g3dSystemVk.hpp"
#endif
#ifndef GVK_G3DSTUBSVK_HPP
#include "GraphicsVulkan/stub/g3dStubsVk.hpp"
#endif
#ifdef MSP_GFX_PROFILING
#ifndef GVK_G3DPROFILERGPUVK_HPP
#include "GraphicsVulkan/G3d/g3dProfilerGpuVk.hpp"
#endif
#endif
#ifndef G3D_GFXOPTIONS_HPP
#include "Graphics/G3d/g3dGfxOptions.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <cstdlib>

using namespace gvk;

namespace
{
	const VkDeviceSize c_RingBlockSize = 4 * 1024 * 1024;

	std::string					l_InitError;
	vkdValidation				l_Validation;		// outlives the device: counts its destruction too
	bool						l_bValidating = false;
	vkdDevice*					l_pDevice = NULL;
	vkdRingBuffer*				l_pRing = NULL;
	vkdBindings*				l_pBindings = NULL;
	g2dSystemVk*				l_pSystem2D = NULL;
	g3dSystemVk*				l_pSystem3D = NULL;
	g3dFragmentCreateVk*		l_pFragmentCreate = NULL;
	mdlFragCreateVk*			l_pMdlFragCreate = NULL;
	g3dSceneRendererCreateVk*	l_pRendererCreate = NULL;
	g3dBakeVk*					l_pBake = NULL;
#ifdef MSP_GFX_PROFILING
	g3dProfilerGpuVk*			l_pProfilerGpu = NULL;
#endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GraphicsVulkanLayer::Init(void* /*i_hWnd*/, const g3dGfxOptions& i_Options)
{
	DBG_ASSERT(!l_pDevice, "GraphicsVulkanLayer::Init called twice");
	l_InitError.clear();
	l_Validation.Reset();
	l_bValidating = false;

	vkdDeviceOptions options;
	options.m_bValidation = i_Options.m_bValidation;
	options.m_pValidation = &l_Validation;
	if (const char* adapter = std::getenv("MSP_GFX_VK_ADAPTER"))
	{
		options.m_AdapterOverride = adapter;
	}
	l_pDevice = vkdDevice::Create(options, l_InitError);
	if (!l_pDevice)
	{
		DBG_LOG("GraphicsVulkan: " << l_InitError);
		return false;
	}
	l_bValidating = l_pDevice->IsValidating();
	l_pRing = new vkdRingBuffer(*l_pDevice, c_RingBlockSize);
	l_pBindings = new vkdBindings(*l_pDevice);
	vkdGlobal::Set(l_pDevice, l_pRing, l_pBindings);

	l_pSystem2D = new g2dSystemVk(*l_pDevice);
	l_pSystem3D = new g3dSystemVk(*l_pDevice);

	l_pFragmentCreate = new g3dFragmentCreateVk;
	g3dFragmentCreate::SetImplementation(l_pFragmentCreate);
	l_pMdlFragCreate = new mdlFragCreateVk;
	mdlFragCreate::SetImplementation(l_pMdlFragCreate);
	return true;
}

void GraphicsVulkanLayer::CleanUp() throw()
{
	if (!l_pDevice)
	{
		return;
	}
	l_pDevice->WaitIdle();
	mdlFragCreate::SetImplementation(NULL);
	g3dFragmentCreate::SetImplementation(NULL);
	delete l_pMdlFragCreate;
	delete l_pFragmentCreate;
	l_pMdlFragCreate = NULL;
	l_pFragmentCreate = NULL;

	delete l_pSystem3D;
	delete l_pSystem2D;
	l_pSystem3D = NULL;
	l_pSystem2D = NULL;

	vkdGlobal::Set(NULL, NULL, NULL);
	delete l_pBindings;
	delete l_pRing;
	l_pBindings = NULL;
	l_pRing = NULL;
	delete l_pDevice;
	l_pDevice = NULL;

	const vkdValidationStats stats = l_Validation.Stats();
	if (l_bValidating)
	{
		DBG_LOG("Vulkan validation: " << stats.m_Errors << " errors, " << stats.m_Warnings << " warnings");
	}
	else
	{
		DBG_LOG("Vulkan validation: off");
	}
}

void GraphicsVulkanLayer::InitGraphics()
{
	l_pRendererCreate = new g3dSceneRendererCreateVk;
	g3dSceneRendererCreate::SetImplementation(l_pRendererCreate);
	l_pBake = new g3dBakeVk;
	g3dBake::SetImplementation(l_pBake);
#ifdef MSP_GFX_PROFILING
	l_pProfilerGpu = new g3dProfilerGpuVk(*l_pDevice);
	g3dProfiler::SetGpu(l_pProfilerGpu);
#endif
}

void GraphicsVulkanLayer::CleanUpGraphics()
{
#ifdef MSP_GFX_PROFILING
	g3dProfiler::SetGpu(NULL);
	delete l_pProfilerGpu;
	l_pProfilerGpu = NULL;
#endif
	g3dBake::SetImplementation(NULL);
	g3dSceneRendererCreate::SetImplementation(NULL);
	delete l_pBake;
	delete l_pRendererCreate;
	l_pBake = NULL;
	l_pRendererCreate = NULL;
}

g2dSystem* GraphicsVulkanLayer::GetSystem2D()
{
	return l_pSystem2D;
}

g3dSystem* GraphicsVulkanLayer::GetSystem3D()
{
	return l_pSystem3D;
}

const std::string& GraphicsVulkanLayer::GetInitError()
{
	return l_InitError;
}

vkdValidationStats GraphicsVulkanLayer::GetValidationStats()
{
	return l_Validation.Stats();
}

bool GraphicsVulkanLayer::IsValidating()
{
	return l_bValidating;
}
