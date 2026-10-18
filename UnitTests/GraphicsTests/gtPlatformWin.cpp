/*****************************************************************************
**  gtPlatformWin.cpp
**
**      see gtPlatform.hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "gtPlatform.hpp"

#include <crtdbg.h>
#include <stdlib.h>
#include <windows.h>

#include <filesystem>

void gtPlatform::SetUpUnattended()
{
	::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);

	_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
}

void* gtPlatform::CreateHiddenWindow(int i_Width, int i_Height)
{
	static bool s_bRegistered = false;
	const wchar_t* c_Class = L"GraphicsTestsHiddenWindow";
	HINSTANCE instance = ::GetModuleHandleW(NULL);
	if (!s_bRegistered)
	{
		WNDCLASSW window_class = {};
		window_class.lpfnWndProc = ::DefWindowProcW;
		window_class.hInstance = instance;
		window_class.lpszClassName = c_Class;
		::RegisterClassW(&window_class);
		s_bRegistered = true;
	}
	//	WS_POPUP: the client area is exactly the size asked for
	return ::CreateWindowExW(0, c_Class, L"GraphicsTests", WS_POPUP, 0, 0, i_Width, i_Height,
							 NULL, NULL, instance, NULL);
}

void gtPlatform::ResizeHiddenWindow(void* i_Window, int i_Width, int i_Height)
{
	::SetWindowPos(static_cast<HWND>(i_Window), NULL, 0, 0, i_Width, i_Height,
				   SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void gtPlatform::DestroyHiddenWindow(void* i_Window)
{
	::DestroyWindow(static_cast<HWND>(i_Window));
}

std::string gtPlatform::RepoRoot()
{
	wchar_t exe[MAX_PATH] = {};
	::GetModuleFileNameW(NULL, exe, MAX_PATH);
	for (std::filesystem::path dir = std::filesystem::path(exe).parent_path(); dir.has_relative_path(); dir = dir.parent_path())
	{
		if (std::filesystem::is_directory(dir / "LibXLT"))
		{
			return (dir / "").string();
		}
	}
	return std::string();
}
