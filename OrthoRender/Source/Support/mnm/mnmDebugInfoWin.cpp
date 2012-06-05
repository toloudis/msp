/*****************************************************************************
**  mnmDebugInfoWin.cpp
**
**      mnmDebugInfo abstracts the call to g2dScreen::SetDebugInfo for easier
**		console porting (this provides the window implementation for this component)
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#include "Support/mnm/mnmDebugInfo.hpp"

#include "Graphics/g2d/g2dWindow.hpp"

namespace
{
	g2dWindow *l_pWindow = NULL;
}

//========================================================================
// Set window for debug display, called from app
//========================================================================
void mnmDebugInfo::SetWindow(g2dWindow* i_pWindow)
{
	l_pWindow = i_pWindow;
}

//========================================================================
//	SetDebugInfo causes the the given text to displayed at the given line
//	of the debug text overlay.  The debug overlay is toggled on and off
//	by the user; currently, this is done with the tilde key.
//	To remove a debug info, call the function with i_Text == NULL.
//========================================================================
void mnmDebugInfo::SetDebugInfo(int i_Line, const char* i_Text)
{
	if (l_pWindow)
		l_pWindow->SetDebugInfo(i_Line, i_Text);
}