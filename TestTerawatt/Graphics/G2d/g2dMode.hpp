/*****************************************************************************
**  g2dMode.hpp
**
**      This is the base class mode for this test project.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_MODE_HPP
#error g2dMode.hpp multiply included
#endif
#define G2D_MODE_HPP

#ifndef APP_MODE_HPP
#include "appMode.hpp"
#endif
#ifndef G2D_FONTHANDLE_HPP
#include "g2dFontHandle.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "g2dRGBColor.hpp"
#endif
#ifndef IT_STRING_HPP
#include "itString.hpp"
#endif

#include <vector>

class g2dWindow;

class g2dMode : public appMode
{
	public:
		//====================================================================
		// Construct with the desired number of debug strings, text color and
		// background color (used on scene clear)
		//====================================================================
		g2dMode( g2dWindow& i_Window,
				int i_nNumDebugStrings, 
				const g2dRGBColor& i_TextColor=g2dRGBColor(0,0,0),
				const g2dRGBColor& i_BackColor=g2dRGBColor(0x30, 0x80, 0xa0) );

		//====================================================================
		// pure-virtual, children must be created
		//====================================================================
		virtual ~g2dMode() = 0;

		//====================================================================
		//	The mode should do it's per frame "work" in the Think function.
		// This simply calls the base class and handles screen captures.
		//====================================================================
		virtual void Think() = 0;

	protected:
		//====================================================================
		// Between thinks, debug strings can be added, they are set to
		// the display vector until Render is called, at which time they 
		// are displayed.
		//====================================================================
		void SetDebugString(int i_nIndex, const itString& i_String );

		//====================================================================
		//	Render is a convenience for child classes which does the usual
		//	rendering stuff like the scScene cull, g3dRenderer::RenderModels,
		//	and so on.  i_Camera can be NULL, in which case no culling is
		//	done (for menus, for instance).
		//	i_ClearColor can also be NULL, for no background clear
		//====================================================================
		void Render();

		//====================================================================
		//====================================================================
		inline g2dWindow& GetWindow();

	private:

		//====================================================================
		// display_debug_text will draw all of the debug strings on each render
		//====================================================================
		void display_debug_text();

		// the debug strings to display
		std::vector<itString> m_DebugStrings;

		// the window to draw to
		g2dWindow& m_Window;

		// the font
		g2dFontHandle m_Font;

		// text and background colors
		g2dRGBColor m_TextColor;
		g2dRGBColor m_BackColor;

};

//====================================================================
//====================================================================
inline g2dWindow& g2dMode::GetWindow()
{
	return m_Window;
}