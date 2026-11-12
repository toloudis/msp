/*****************************************************************************
**  GraphicsOptiXLayer.cpp
**
**      See GraphicsOptiXLayer.hpp. Init: CUDA, OptiX, optxContext, then
**	GraphicsVulkanLayer on the same GPU, then the OptiX scene renderer
**	creator over Vulkan's. CleanUp reverses it: OptiX's objects, then the
**	Vulkan layer, the OptiX context last.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_GRAPHICSOPTIXLAYER_HPP
#include "GraphicsOptiX/GraphicsOptiXLayer.hpp"
#endif
#ifndef GOX_OPTXLOADER_HPP
#include "GraphicsOptiX/optx/optxLoader.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GOX_CULOADER_HPP
#include "GraphicsOptiX/cu/cuLoader.hpp"
#endif
#ifndef GOX_G3DSCENERENDERERCREATEOPTIX_HPP
#include "GraphicsOptiX/G3d/private/g3dSceneRendererCreateOptiX.hpp"
#endif
#ifndef GOX_G3DGASCACHEOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dGasCacheOptiX.hpp"
#endif
#ifndef GVK_GRAPHICSVULKANLAYER_HPP
#include "GraphicsVulkan/GraphicsVulkanLayer.hpp"
#endif
#ifndef GVK_G3DVKBUFFERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkBufferUtil.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifdef MSP_GFX_PROFILING
#ifndef GOX_G3DPROFILERGPUOPTIX_HPP
#include "GraphicsOptiX/G3d/g3dProfilerGpuOptiX.hpp"
#endif
#endif
#ifndef G3D_GFXOPTIONS_HPP
#include "Graphics/G3d/g3dGfxOptions.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <string>

using namespace gox;

namespace
{
	std::string						l_InitError;
	bool							l_bInitialized = false;
	g3dSceneRendererCreateOptiX*	l_pRendererCreator = NULL;
	g3dGasCacheOptiX*				l_pGasCache = NULL;
	int								l_nOptixErrors = 0;		// kept after CleanUp
	int								l_nOptixWarnings = 0;
#ifdef MSP_GFX_PROFILING
	g3dProfilerGpuOptiX*			l_pProfilerGpu = NULL;	// over Vulkan's timer
#endif

	void unload_optix()
	{
		optxContext::Destroy();
		optxLoader::Unload();
		cuLoader::Unload();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool GraphicsOptiXLayer::Init(void* i_hWnd, const g3dGfxOptions& i_Options)
{
	DBG_ASSERT(!l_bInitialized, "GraphicsOptiXLayer::Init called twice");
	l_InitError.clear();
	l_nOptixErrors = 0;
	l_nOptixWarnings = 0;

	if (!cuLoader::Load(l_InitError))
	{
		DBG_LOG("GraphicsOptiX: " << l_InitError);
		return false;
	}
	if (!optxLoader::Load(l_InitError))
	{
		DBG_LOG("GraphicsOptiX: " << l_InitError);
		cuLoader::Unload();
		return false;
	}
	if (!optxContext::Create(i_Options.m_bValidation, l_InitError))
	{
		DBG_LOG("GraphicsOptiX: " << l_InitError);
		optxLoader::Unload();
		cuLoader::Unload();
		return false;
	}

	//	Vulkan on the GPU CUDA chose, sharing memory and semaphores with it
	GraphicsVulkanLayer::Interop interop;
	interop.m_DeviceUUID = optxContext::GetDeviceUUID();
	interop.m_DeviceName = "the CUDA device \"" + optxContext::GetDeviceName() + "\"";
	interop.m_bExternalMemory = true;
	if (!GraphicsVulkanLayer::Init(i_hWnd, i_Options, interop))
	{
		l_InitError = GraphicsVulkanLayer::GetInitError();
		DBG_LOG("GraphicsOptiX: " << l_InitError);
		unload_optix();
		return false;
	}
	l_bInitialized = true;

	//	Static meshes in exportable memory, so GAS are built from them in place
	gvk::g3dVkBufferUtil::ExportStaticGeometry(true);
	l_pGasCache = new g3dGasCacheOptiX(gvk::vkdGlobal::Device(), gvk::vkdGlobal::Context());
	g3dGasCacheOptiX::Set(l_pGasCache);

	l_pRendererCreator = new g3dSceneRendererCreateOptiX(g3dSceneRendererCreate::SetImplementation(NULL));
	g3dSceneRendererCreate::SetImplementation(l_pRendererCreator);

	return true;
}

void GraphicsOptiXLayer::CleanUp() throw()
{
	if (!l_bInitialized)
	{
		return;
	}
	//	Vulkan's creator back, for its own CleanUp to remove
	g3dSceneRendererCreate::SetImplementation(l_pRendererCreator->GetInner());
	delete l_pRendererCreator;
	l_pRendererCreator = NULL;

	//	Before Vulkan frees the memory the GAS were built from
	g3dGasCacheOptiX::Set(NULL);
	delete l_pGasCache;
	l_pGasCache = NULL;
	gvk::g3dVkBufferUtil::ExportStaticGeometry(false);

	GraphicsVulkanLayer::CleanUp();

	l_nOptixErrors = optxContext::GetErrorCount();
	l_nOptixWarnings = optxContext::GetWarningCount();
	DBG_LOG("OptiX messages: " << l_nOptixErrors << " errors, " << l_nOptixWarnings << " warnings");
	unload_optix();
	l_bInitialized = false;
}

void GraphicsOptiXLayer::InitGraphics()
{
	GraphicsVulkanLayer::InitGraphics();
#ifdef MSP_GFX_PROFILING
	//	CUDA stream times too (cuda_ms), around the GPU timer Vulkan just installed
	if (g3dProfilerGpu* vulkan_timer = g3dProfiler::GetGpu())
	{
		l_pProfilerGpu = new g3dProfilerGpuOptiX(vulkan_timer);
		g3dProfiler::SetGpu(l_pProfilerGpu);
	}
#endif
}

void GraphicsOptiXLayer::CleanUpGraphics()
{
#ifdef MSP_GFX_PROFILING
	//	Vulkan's timer back, for its own CleanUpGraphics to remove
	if (l_pProfilerGpu)
	{
		g3dProfiler::SetGpu(l_pProfilerGpu->GetInner());
		delete l_pProfilerGpu;
		l_pProfilerGpu = NULL;
	}
#endif
	GraphicsVulkanLayer::CleanUpGraphics();
}

g2dSystem* GraphicsOptiXLayer::GetSystem2D()
{
	return GraphicsVulkanLayer::GetSystem2D();
}

g3dSystem* GraphicsOptiXLayer::GetSystem3D()
{
	return GraphicsVulkanLayer::GetSystem3D();
}

const char* GraphicsOptiXLayer::GetName()
{
	return "OptiX";
}

const char* GraphicsOptiXLayer::GetCodeName()
{
	return "optix";
}

const std::string& GraphicsOptiXLayer::GetInitError()
{
	return l_InitError;
}

void GraphicsOptiXLayer::GetOptixMessageCounts(int& o_nErrors, int& o_nWarnings)
{
	if (l_bInitialized)
	{
		o_nErrors = optxContext::GetErrorCount();
		o_nWarnings = optxContext::GetWarningCount();
	}
	else
	{
		o_nErrors = l_nOptixErrors;
		o_nWarnings = l_nOptixWarnings;
	}
}
