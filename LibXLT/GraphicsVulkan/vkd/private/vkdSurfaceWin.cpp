/*****************************************************************************
**  vkdSurfaceWin.cpp
**
**      Windows window surfaces (see vkdSurface.hpp).
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_VKDSURFACE_HPP
#include "GraphicsVulkan/vkd/vkdSurface.hpp"
#endif

#include <windows.h>

namespace gvk
{
	namespace
	{
		//	VK_KHR_win32_surface, declared here so the rest of GraphicsVulkan
		//	never sees windows.h.
		struct Win32SurfaceCreateInfo
		{
			VkStructureType	sType;
			const void*		pNext;
			VkFlags			flags;
			HINSTANCE		hinstance;
			HWND			hwnd;
		};
		const VkStructureType c_Win32SurfaceCreateInfoType = static_cast<VkStructureType>(1000009000);
		typedef VkResult (VKAPI_PTR *CreateWin32Surface)(VkInstance, const Win32SurfaceCreateInfo*,
														  const VkAllocationCallbacks*, VkSurfaceKHR*);
	}

	const char* vkdPlatformSurfaceExtension()
	{
		return "VK_KHR_win32_surface";
	}

	VkSurfaceKHR vkdCreateSurface(VkInstance i_Instance, void* i_NativeWindow)
	{
		CreateWin32Surface create = reinterpret_cast<CreateWin32Surface>(
			vkGetInstanceProcAddr(i_Instance, "vkCreateWin32SurfaceKHR"));
		if (!create || !i_NativeWindow)
		{
			return VK_NULL_HANDLE;
		}
		Win32SurfaceCreateInfo info = {};
		info.sType = c_Win32SurfaceCreateInfoType;
		info.hinstance = ::GetModuleHandleW(NULL);
		info.hwnd = static_cast<HWND>(i_NativeWindow);
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		if (create(i_Instance, &info, NULL, &surface) != VK_SUCCESS)
		{
			return VK_NULL_HANDLE;
		}
		return surface;
	}

	void vkdWindowClientSize(void* i_NativeWindow, int& o_Width, int& o_Height)
	{
		RECT rect = {};
		if (i_NativeWindow && ::GetClientRect(static_cast<HWND>(i_NativeWindow), &rect))
		{
			o_Width = rect.right - rect.left;
			o_Height = rect.bottom - rect.top;
		}
		else
		{
			o_Width = 0;
			o_Height = 0;
		}
	}

	bool vkdWindowIsOpen(void* i_NativeWindow)
	{
		return i_NativeWindow && ::IsWindow(static_cast<HWND>(i_NativeWindow)) != 0;
	}

	void vkdSetWindowTitle(void* i_NativeWindow, const wchar_t* i_Title)
	{
		::SetWindowTextW(static_cast<HWND>(i_NativeWindow), i_Title);
	}
}
