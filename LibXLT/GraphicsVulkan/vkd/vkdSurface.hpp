/*****************************************************************************
**  vkdSurface.hpp
**
**      The platform part of window surfaces (PAC style): the instance
**	extension a surface needs, creating one for a native window, and the
**	few native window queries the swap chain needs.
**	Implemented per platform (vkdSurfaceWin.cpp on Windows).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDSURFACE_HPP
#error vkdSurface.hpp multiply included
#endif
#define GVK_VKDSURFACE_HPP

#include <volk.h>
#include <string>

namespace gvk
{
	//	The platform's surface instance extension ("VK_KHR_win32_surface").
	const char* vkdPlatformSurfaceExtension();

	//	A surface for a native window (HWND on Windows), or VK_NULL_HANDLE.
	VkSurfaceKHR vkdCreateSurface(VkInstance i_Instance, void* i_NativeWindow);

	//	Native window queries the swap chain code needs.
	void vkdWindowClientSize(void* i_NativeWindow, int& o_Width, int& o_Height);
	bool vkdWindowIsOpen(void* i_NativeWindow);
	//	i_Title is UTF-8.
	void vkdSetWindowTitle(void* i_NativeWindow, const std::string& i_Title);
}
