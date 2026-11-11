/*****************************************************************************
**  g2dWindowPACWin.hpp
**
**      The Windows PAC for g2dWindowPAC (see g2dWindowPAC.hpp): the client
**	area through GetClientRect, filled with GDI.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G2DWINDOWPACWIN_HPP
#error g2dWindowPACWin.hpp multiply included
#endif
#define GOX_G2DWINDOWPACWIN_HPP

namespace gox
{
	namespace g2dWindowPAC
	{
		bool GetClientSize(void* i_hWnd, int& o_Width, int& o_Height);
		void FillClient(void* i_hWnd, unsigned char i_Red, unsigned char i_Green, unsigned char i_Blue);
	}
}
