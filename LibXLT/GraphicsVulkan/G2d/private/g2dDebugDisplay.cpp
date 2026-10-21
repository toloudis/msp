/****************************************************************************\
**	g2dDebugDisplay.cpp
**
**		g2dDebugDisplay.hpp holds lines of text for debug display
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G2DDEBUGDISPLAY_HPP
#include "GraphicsVulkan/G2d/g2dDebugDisplay.hpp"
#endif
#ifndef G2D_FONTUTIL_HPP
#include "Graphics/G2d/g2dFontUtil.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/G2d/g2dWindow.hpp"
#endif

#include <algorithm>


namespace gvk
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g2dDebugDisplay::g2dDebugDisplay(g2dWindow& i_Window)
	:	m_Window(i_Window),
		m_bEnabled(false)
	{
		//	DX11's: the font name is left empty (the bitmap font, whatever it's called)
		itString font_loc_s;
		m_DebugFont = g2dFontUtil::LoadFont( font_loc_s, 14 );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g2dDebugDisplay::~g2dDebugDisplay()
	{
		g2dFontUtil::ReleaseFont(m_DebugFont);
	}

	//--------------------------------------------------------------------
	//	Render renders the debug display, if necessary
	//--------------------------------------------------------------------
	void g2dDebugDisplay::Render()
	{
		if ( m_bEnabled )
		{
			std::map<int, itString>::iterator it = m_DebugInfo.begin();
			std::map<int, itString>::iterator end = m_DebugInfo.end();

			g2dRGBColor text_color(0x00,0xff,0x00);
			while ( it != end )
			{
				const int l_X_OFFSET = 10;
				const int l_Y_OFFSET = 15;
				const int l_Y_LINE_SPACE = 15;
				int y = (it->first * l_Y_LINE_SPACE) + l_Y_OFFSET;
				int x = l_X_OFFSET;

				m_Window.DrawText(	x, y, m_DebugFont, it->second, text_color);
				++it;
			}
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void g2dDebugDisplay::SetDebugInfo(int i_Line, const char* i_Text)
	{
		if ( i_Text )
			m_DebugInfo[i_Line] = itString(i_Text);
		else
			m_DebugInfo.erase(m_DebugInfo.find(i_Line));
	}
}
