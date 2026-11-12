/*****************************************************************************
**  cuExternalMemoryCache.cpp
**
**      See cuExternalMemoryCache.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_CUEXTERNALMEMORYCACHE_HPP
#include "GraphicsOptiX/cu/cuExternalMemoryCache.hpp"
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
#ifndef GVK_VKDBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdBuffer.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_VKDEXTERNAL_HPP
#include "GraphicsVulkan/vkd/vkdExternal.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gox
{
	cuExternalMemoryCache::cuExternalMemoryCache(gvk::vkdDevice& i_Device)
		: m_Device(i_Device)
		, m_HookId(0)
		, m_nImports(0)
	{
		m_HookId = m_Device.AddMemoryFreeHook([this](VkDeviceMemory i_Memory) { forget(i_Memory); });
	}

	cuExternalMemoryCache::~cuExternalMemoryCache()
	{
		m_Device.RemoveMemoryFreeHook(m_HookId);
		std::lock_guard<std::mutex> lock(m_Lock);
		if (!m_Blocks.empty())
		{
			optxContext::MakeCurrent();
			for (auto& block : m_Blocks)
			{
				CU_CHECK(cu().cuDestroyExternalMemory(block.second.m_Memory));
			}
			m_Blocks.clear();
		}
	}

	CUdeviceptr cuExternalMemoryCache::Map(const gvk::vkdBuffer& i_Buffer)
	{
		if (!i_Buffer.IsExportable())
		{
			DBG_LOG("GraphicsOptiX: a buffer that isn't exportable can't be read by CUDA");
			return 0;
		}
		const VkDeviceMemory memory = i_Buffer.Memory();
		const VkDeviceSize offset = i_Buffer.MemoryOffset();
		//	Asked before taking m_Lock: VMA takes its own locks, and frees
		//	blocks (calling forget) while holding them
		const VkDeviceSize size = i_Buffer.MemoryBlockSize();
		const bool dedicated = i_Buffer.IsDedicatedMemory();

		std::lock_guard<std::mutex> lock(m_Lock);
		auto found = m_Blocks.find(memory);
		if (found != m_Blocks.end())
		{
			return found->second.m_Ptr + offset;
		}

		void* handle = gvk::vkdExportMemoryHandle(m_Device.Device(), memory);
		if (!handle)
		{
			DBG_LOG("GraphicsOptiX: Vulkan gave no handle for a memory block");
			return 0;
		}
		CUDA_EXTERNAL_MEMORY_HANDLE_DESC desc = {};
		cuSetPlatformMemoryHandle(desc, handle);
		desc.size = size;
		desc.flags = dedicated ? CUDA_EXTERNAL_MEMORY_DEDICATED : 0;
		CUexternalMemory external = NULL;
		const bool imported = !CU_CHECK(cu().cuImportExternalMemory(&external, &desc));
		gvk::vkdCloseExternalHandle(handle);	// CUDA keeps its own reference
		if (!imported)
		{
			return 0;
		}

		CUDA_EXTERNAL_MEMORY_BUFFER_DESC buffer_desc = {};
		buffer_desc.offset = 0;
		buffer_desc.size = size;
		CUdeviceptr ptr = 0;
		if (CU_CHECK(cu().cuExternalMemoryGetMappedBuffer(&ptr, external, &buffer_desc)))
		{
			cu().cuDestroyExternalMemory(external);
			return 0;
		}
		m_Blocks[memory] = Block{ external, ptr };
		++m_nImports;
		return ptr + offset;
	}

	void cuExternalMemoryCache::forget(VkDeviceMemory i_Memory)
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		auto found = m_Blocks.find(i_Memory);
		if (found == m_Blocks.end())
		{
			return;
		}
		//	The mapped pointer goes with the import (CUDA frees it)
		optxContext::MakeCurrent();
		CU_CHECK(cu().cuDestroyExternalMemory(found->second.m_Memory));
		m_Blocks.erase(found);
	}

	size_t cuExternalMemoryCache::GetBlockCount() const
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		return m_Blocks.size();
	}

	int cuExternalMemoryCache::GetImportCount() const
	{
		std::lock_guard<std::mutex> lock(m_Lock);
		return m_nImports;
	}
}
