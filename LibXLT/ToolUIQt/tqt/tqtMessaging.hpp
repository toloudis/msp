/*****************************************************************************
**  tqtMessaging.hpp
**
**     Handles keypresses, converting into commands
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef TQT_MESSAGING_HPP
#error tqtMessaging.hpp multiply included
#endif
#define TQT_MESSAGING_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef QT_FINISH_PORT

//============================================================================
//============================================================================
namespace tqtMessaging
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
	// Add a QWidget to list of windows that want to handle 
	//	hot key events.
	//--------------------------------------------------------------------
	void WindowWantsHotKeys(QWidget* i_pWindow);

	//--------------------------------------------------------------------
	// Return true if window wants to receive hot keys.
	//--------------------------------------------------------------------
	bool WantsHotKeys(QWidget* i_pWindow);

	//--------------------------------------------------------------------
	// Return true if there are any windows that want to process hot keys.
	//--------------------------------------------------------------------
	bool ShouldProcessHotKeys();
}

#endif // USE_QT
