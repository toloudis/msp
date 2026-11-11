/*****************************************************************************
**  g2dWindowPACWin.cpp
**
**      See g2dWindowPACWin.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G2DWINDOWPAC_HPP
#include "GraphicsOptiX/G2d/private/g2dWindowPAC.hpp"
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace gox
{
	namespace g2dWindowPAC
	{
		bool GetClientSize(void* i_hWnd, int& o_Width, int& o_Height)
		{
			RECT rect;
			if (!i_hWnd || !::GetClientRect(static_cast<HWND>(i_hWnd), &rect))
			{
				return false;
			}
			o_Width = rect.right - rect.left;
			o_Height = rect.bottom - rect.top;
			return true;
		}

		void FillClient(void* i_hWnd, unsigned char i_Red, unsigned char i_Green, unsigned char i_Blue)
		{
			HWND hwnd = static_cast<HWND>(i_hWnd);
			RECT rect;
			if (!hwnd || !::GetClientRect(hwnd, &rect))
			{
				return;
			}
			HDC dc = ::GetDC(hwnd);
			if (!dc)
			{
				return;
			}
			HBRUSH brush = ::CreateSolidBrush(RGB(i_Red, i_Green, i_Blue));
			::FillRect(dc, &rect, brush);
			::DeleteObject(brush);
			::ReleaseDC(hwnd, dc);
		}
	}
}
