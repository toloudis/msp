/*****************************************************************************
**  g2dMode.cpp
**
**      See g2dMode.hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "g2dMode.hpp"

#include "appModeMgr.hpp"
#include "appSimTime.hpp"
#include "dbgLog.hpp"
#include "g2dFontUtil.hpp"
#include "g2dWindow.hpp"
#include "inDeviceMgr.hpp"

//====================================================================
//====================================================================
g2dMode::g2dMode(g2dWindow& i_Window,
				int i_nNumDebugStrings, 
				const g2dRGBColor& i_TextColor/*=g2dRGBColor(235,20,80)*/,
				const g2dRGBColor& i_BackColor/*=g2dRGBColor(0,0,0)*/ )
:	m_DebugStrings(i_nNumDebugStrings),
	m_Window(i_Window),
	m_Font( g2dFontUtil::LoadFont(itString("Arial"), 14) ),
	m_TextColor( i_TextColor ),
	m_BackColor( i_BackColor )
{
}

//====================================================================
//====================================================================
// virtual
g2dMode::~g2dMode()
{
	g2dFontUtil::ReleaseFont(m_Font);
}

//====================================================================
//	The mode should do it's per frame "work" in the Think function.
//====================================================================
void g2dMode::Think()
{
	// base class think first
	appMode::Think();

	// input think
	inDeviceMgr::Think();

	// Get the keyboard pointer.
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();

	// Space bar is used to exit modes, by default
	if (pKeyboard->IsReleased(inKeys::e_SPACE))
	{
		SetTerminateCondition(e_TerminateAndRemove);
		return;
	}

	// ESC is used to exit the program immediately, by default
	if ( pKeyboard->IsReleased( inKeys::e_ESC ) )
	{
		appModeMgr::Clear();
		SetTerminateCondition(e_TerminateAndRemove);
		return;
	}

	m_Window.Clear( g2dRGBColor( 0, 0, 200 ) );
	//g2dScreenDrawUtil::Clear( g2dRGBColor( 0, 0, 200 ) );

}

//====================================================================
// Between thinks, debug strings can be added, they are set to
// the display vector until Render is called, at which time they 
// are displayed.
//====================================================================
void g2dMode::SetDebugString( int i_nIndex, const itString& i_String )
{

	DBG_ASSERT0(i_nIndex < m_DebugStrings.size(), "Invalid debug string index, allocate more!" );

	m_DebugStrings[i_nIndex] = i_String;
}

//====================================================================
//	Render is a convenience for child classes which does the usual
//	rendering stuff like the scScene cull, g3dRenderer::RenderModels,
//	and so on.  i_Camera can be NULL, in which case no culling is
//	done (for menus, for instance).
//====================================================================
void g2dMode::Render()
{

	//	flip screen from last frame
	//g2dScreen::EndScene();
	m_Window.EndScene();

}

//====================================================================
// display_debug_text will draw all of the debug strings on each render
//====================================================================
void g2dMode::display_debug_text()
{
	int num_lines = m_DebugStrings.size();

	int i;
	for( i = 0 ; i < num_lines; i++ )
	{
		m_Window.DrawText(	10,
							(i * 17) + 10,
							m_Font,
							m_DebugStrings[i],
							m_TextColor);

		// text_color.SetRed(text_color.GetRed() + 10);
	}
}