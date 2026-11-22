/*****************************************************************************
**  vkdExternal.hpp
**
**      The platform part of sharing memory and semaphores with another API
**	(CUDA, for GraphicsOptiX), PAC style: the device extensions it needs,
**	the handle types, and getting an OS handle for a block of device
**	memory or a semaphore. Implemented per platform (vkdExternalWin.cpp on
**	Windows: opaque Win32 handles).
**
**	A handle these functions return is the caller's: it is closed with
**	vkdCloseExternalHandle once the other API has imported it (CUDA
**	doesn't take ownership of a Win32 handle).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDEXTERNAL_HPP
#error vkdExternal.hpp multiply included
#endif
#define GVK_VKDEXTERNAL_HPP

#include <volk.h>
#include <string>
#include <vector>

namespace gvk
{
	//	The device extensions for external memory and semaphores
	//	("VK_KHR_external_memory_win32", "VK_KHR_external_semaphore_win32").
	const std::vector<std::string>& vkdPlatformExternalExtensions();

	//	The handle types memory and semaphores are exported as
	VkExternalMemoryHandleTypeFlagBits		vkdPlatformMemoryHandleType();
	VkExternalSemaphoreHandleTypeFlagBits	vkdPlatformSemaphoreHandleType();

	//	A new OS handle (HANDLE on Windows) for memory allocated with
	//	VkExportMemoryAllocateInfo, or for a semaphore created with
	//	VkExportSemaphoreCreateInfo, both of the platform handle type;
	//	NULL if the driver refuses.
	void* vkdExportMemoryHandle(VkDevice i_Device, VkDeviceMemory i_Memory);
	void* vkdExportSemaphoreHandle(VkDevice i_Device, VkSemaphore i_Semaphore);

	//	Closes a handle from the functions above (NULL does nothing).
	void vkdCloseExternalHandle(void* i_Handle);
}
