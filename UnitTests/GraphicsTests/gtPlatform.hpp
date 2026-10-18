/*****************************************************************************
**  gtPlatform.hpp
**
**      Platform setup for running the unit tests unattended.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

namespace gtPlatform
{
	//	SetUpUnattended makes runtime errors (CRT asserts, abort(),
	//	crashes) print to stderr and fail the process instead of opening
	//	a dialog, so a CI run can't hang waiting for someone to click OK.
	void SetUpUnattended();

	//	A plain native window that is never shown (HWND on Windows), for
	//	swap chain tests. Destroy it with DestroyHiddenWindow.
	void* CreateHiddenWindow(int i_Width, int i_Height);
	void ResizeHiddenWindow(void* i_Window, int i_Width, int i_Height);
	void DestroyHiddenWindow(void* i_Window);
}
