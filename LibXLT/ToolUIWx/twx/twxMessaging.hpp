/*****************************************************************************
**  twxMessaging.hpp
**
**     Handles keypresses, converting into commands
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef TWX_MESSAGING_HPP
#error twxMessaging.hpp multiply included
#endif
#define TWX_MESSAGING_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
namespace twxMessaging
{
	//--------------------------------------------------------------------
	// Convert just the wxWidgets key code to a string,
	//	this will not include the modifier keys.
	//--------------------------------------------------------------------
	bool ConvertKeyCodeToString(int i_KeyCode, 
								 std::string& o_HotKeyString);

	//--------------------------------------------------------------------
	// Convert a key event to a string representing a hot key
	//--------------------------------------------------------------------
	bool ConvertKeyEventToString(wxKeyEvent& i_Event, 
								 std::string& o_HotKeyString);

	//--------------------------------------------------------------------
	// Pass in a key event to check to see if it can be converted
	//	to a command
	//--------------------------------------------------------------------
	bool ProcessKeyEvent(wxKeyEvent& i_Event);

	//--------------------------------------------------------------------
	// Add a wxWindow to list of windows that want to handle 
	//	hot key events.
	//--------------------------------------------------------------------
	void WindowWantsHotKeys(wxWindow* i_pWindow);

	//--------------------------------------------------------------------
	// Return true if window wants to receive hot keys.
	//--------------------------------------------------------------------
	bool WantsHotKeys(wxWindow* i_pWindow);

	//--------------------------------------------------------------------
	// Return true if there are any windows that want to process hot keys.
	//--------------------------------------------------------------------
	bool ShouldProcessHotKeys();
}

#endif // USE_WXWIDGETS
