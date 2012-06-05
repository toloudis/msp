/****************************************************************************\
**	g2dDebugDisplay.cpp
**
**		g2dDebugDisplay.hpp holds lines of text for debug display
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/g2d/g2dDebugDisplay.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/Fs/fsLocator.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Graphics/g2d/g2dRGBColor.hpp"
#include "Graphics/g2d/g2dWindow.hpp"

#include <algorithm>


//============================================================================
//============================================================================
namespace
{
	const char* c_FontName = "font-lucd00.png";
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
g2dDebugDisplay::g2dDebugDisplay(g2dWindow& i_Window)
:	m_Window(i_Window),
	m_bEnabled(false)
{
//	m_DebugFont = g2dFontUtil::LoadFont( itString("Arial"), 14 );

	//fsLocator font_loc;
	//g2dFontUtil::GetGlobalFontBitmap(font_loc);
	itString font_loc_s;
	//// First user def path happens to be where fontMap is
	//// Need to change this code if e_ExeArt no longer equals to e_FirstUserDefPath
	//font_loc.Push( gfPaths::GetPath(gfPaths::e_FirstUserDefPath) ); 
	//font_loc.Push( c_FontName );
	//fsFileUtil::LocatorToUnicodeString(font_loc, font_loc_s);
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

