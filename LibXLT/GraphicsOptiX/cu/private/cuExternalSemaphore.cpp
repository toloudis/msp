/*****************************************************************************
**  cuExternalSemaphore.cpp
**
**      See cuExternalSemaphore.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_CUEXTERNALSEMAPHORE_HPP
#include "GraphicsOptiX/cu/cuExternalSemaphore.hpp"
#endif
#ifndef GOX_CUEXTERNAL_HPP
#include "GraphicsOptiX/cu/cuExternal.hpp"
#endif
#ifndef GOX_CULOADER_HPP
#include "GraphicsOptiX/cu/cuLoader.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GVK_VKDEXTERNAL_HPP
#include "GraphicsVulkan/vkd/vkdExternal.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gox
{
	cuExternalSemaphore::cuExternalSemaphore()
		: m_Semaphore(NULL)
	{
	}

	cuExternalSemaphore::~cuExternalSemaphore()
	{
		Release();
	}

	bool cuExternalSemaphore::Import(VkDevice i_Device, VkSemaphore i_Semaphore)
	{
		Release();
		void* handle = gvk::vkdExportSemaphoreHandle(i_Device, i_Semaphore);
		if (!handle)
		{
			DBG_LOG("GraphicsOptiX: Vulkan gave no handle for a timeline semaphore");
			return false;
		}
		CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC desc = {};
		cuSetPlatformTimelineHandle(desc, handle);
		const bool imported = !CU_CHECK(cu().cuImportExternalSemaphore(&m_Semaphore, &desc));
		gvk::vkdCloseExternalHandle(handle);	// CUDA keeps its own reference
		if (!imported)
		{
			m_Semaphore = NULL;
		}
		return imported;
	}

	void cuExternalSemaphore::Release()
	{
		if (m_Semaphore)
		{
			CU_CHECK(cu().cuDestroyExternalSemaphore(m_Semaphore));
			m_Semaphore = NULL;
		}
	}

	bool cuExternalSemaphore::Wait(uint64_t i_Value)
	{
		CUDA_EXTERNAL_SEMAPHORE_WAIT_PARAMS params = {};
		params.params.fence.value = i_Value;
		return m_Semaphore && !CU_CHECK(cu().cuWaitExternalSemaphoresAsync(&m_Semaphore, &params, 1, optxContext::GetStream()));
	}

	bool cuExternalSemaphore::Signal(uint64_t i_Value)
	{
		CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS params = {};
		params.params.fence.value = i_Value;
		return m_Semaphore && !CU_CHECK(cu().cuSignalExternalSemaphoresAsync(&m_Semaphore, &params, 1, optxContext::GetStream()));
	}
}
