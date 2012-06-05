/*****************************************************************************
**  mnmDebugInfo.hpp
**
**      mnmDebugInfo abstracts the call to g2dScreen::SetDebugInfo for easier
**		console porting
**
**	Extra Large Technology
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#ifdef MNM_DEBUGINFO_HPP
#error mnmDebugInfo.hpp multiply included
#endif
#define MNM_DEBUGINFO_HPP

class g2dWindow;

namespace mnmDebugInfo
{
	//========================================================================
	// Set window for debug display, called from app
	//========================================================================
	void SetWindow(g2dWindow* i_pWindow);

	//========================================================================
	//	SetDebugInfo causes the the given text to displayed at the given line
	//	of the debug text overlay.  The debug overlay is toggled on and off
	//	by the user; currently, this is done with the tilde key.
	//	To remove a debug info, call the function with i_Text == NULL.
	//========================================================================
	void SetDebugInfo(int i_Line, const char* i_Text);
}