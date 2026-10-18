/*****************************************************************************
**  vkdSurface.hpp
**
**      The platform part of window surfaces (PAC style): the instance
**	extension a surface needs and creating one for a native window.
**	Implemented per platform (vkdSurfaceWin.cpp on Windows).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDSURFACE_HPP
#error vkdSurface.hpp multiply included
#endif
#define GVK_VKDSURFACE_HPP

#include <volk.h>

namespace gvk
{
	//	The platform's surface instance extension ("VK_KHR_win32_surface").
	const char* vkdPlatformSurfaceExtension();

	//	A surface for a native window (HWND on Windows), or VK_NULL_HANDLE.
	VkSurfaceKHR vkdCreateSurface(VkInstance i_Instance, void* i_NativeWindow);
}
