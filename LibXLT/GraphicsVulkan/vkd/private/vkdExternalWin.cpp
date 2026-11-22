/*****************************************************************************
**  vkdExternalWin.cpp
**
**      Windows external memory and semaphores (see vkdExternal.hpp):
**	opaque Win32 handles.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDEXTERNAL_HPP
#include "GraphicsVulkan/vkd/vkdExternal.hpp"
#endif

#include <windows.h>

namespace gvk
{
	namespace
	{
		//	VK_KHR_external_memory_win32 and VK_KHR_external_semaphore_win32,
		//	declared here so the rest of GraphicsVulkan never sees windows.h
		struct MemoryGetWin32HandleInfo
		{
			VkStructureType						sType;
			const void*							pNext;
			VkDeviceMemory						memory;
			VkExternalMemoryHandleTypeFlagBits	handleType;
		};
		struct SemaphoreGetWin32HandleInfo
		{
			VkStructureType							sType;
			const void*								pNext;
			VkSemaphore								semaphore;
			VkExternalSemaphoreHandleTypeFlagBits	handleType;
		};
		const VkStructureType c_MemoryGetWin32HandleInfoType = static_cast<VkStructureType>(1000073003);
		const VkStructureType c_SemaphoreGetWin32HandleInfoType = static_cast<VkStructureType>(1000078003);
		typedef VkResult (VKAPI_PTR *GetMemoryWin32Handle)(VkDevice, const MemoryGetWin32HandleInfo*, HANDLE*);
		typedef VkResult (VKAPI_PTR *GetSemaphoreWin32Handle)(VkDevice, const SemaphoreGetWin32HandleInfo*, HANDLE*);
	}

	const std::vector<std::string>& vkdPlatformExternalExtensions()
	{
		static const std::vector<std::string> s_Extensions =
		{
			"VK_KHR_external_memory_win32",
			"VK_KHR_external_semaphore_win32",
		};
		return s_Extensions;
	}

	VkExternalMemoryHandleTypeFlagBits vkdPlatformMemoryHandleType()
	{
		return VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
	}

	VkExternalSemaphoreHandleTypeFlagBits vkdPlatformSemaphoreHandleType()
	{
		return VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT;
	}

	void* vkdExportMemoryHandle(VkDevice i_Device, VkDeviceMemory i_Memory)
	{
		GetMemoryWin32Handle get = reinterpret_cast<GetMemoryWin32Handle>(
			vkGetDeviceProcAddr(i_Device, "vkGetMemoryWin32HandleKHR"));
		if (!get || !i_Memory)
		{
			return NULL;
		}
		MemoryGetWin32HandleInfo info = {};
		info.sType = c_MemoryGetWin32HandleInfoType;
		info.memory = i_Memory;
		info.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
		HANDLE handle = NULL;
		if (get(i_Device, &info, &handle) != VK_SUCCESS)
		{
			return NULL;
		}
		return handle;
	}

	void* vkdExportSemaphoreHandle(VkDevice i_Device, VkSemaphore i_Semaphore)
	{
		GetSemaphoreWin32Handle get = reinterpret_cast<GetSemaphoreWin32Handle>(
			vkGetDeviceProcAddr(i_Device, "vkGetSemaphoreWin32HandleKHR"));
		if (!get || !i_Semaphore)
		{
			return NULL;
		}
		SemaphoreGetWin32HandleInfo info = {};
		info.sType = c_SemaphoreGetWin32HandleInfoType;
		info.semaphore = i_Semaphore;
		info.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT;
		HANDLE handle = NULL;
		if (get(i_Device, &info, &handle) != VK_SUCCESS)
		{
			return NULL;
		}
		return handle;
	}

	void vkdCloseExternalHandle(void* i_Handle)
	{
		if (i_Handle)
		{
			::CloseHandle(static_cast<HANDLE>(i_Handle));
		}
	}
}
