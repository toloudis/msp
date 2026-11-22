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

#ifdef USE_QT

//============================================================================
//============================================================================
namespace tqtMessaging
{
	//--------------------------------------------------------------------
	// A widget with this property set to true never processes hot keys
	//	(it takes the keys itself, like the hot key editor).
	//--------------------------------------------------------------------
	const char* const c_NoHotKeysProperty = "tqtNoHotKeys";

	//--------------------------------------------------------------------
	// Convert just the Qt key code to a string,
	//	this will not include the modifier keys. i_bKeypad is true when
	//	the key came from the numeric keypad.
	//--------------------------------------------------------------------
	bool ConvertKeyCodeToString(int i_KeyCode,
								bool i_bKeypad,
								std::string& o_HotKeyString);

	//--------------------------------------------------------------------
	// Convert a key event to a string representing a hot key
	//--------------------------------------------------------------------
	bool ConvertKeyEventToString(const QKeyEvent& i_Event,
								 std::string& o_HotKeyString);

	//--------------------------------------------------------------------
	// Pass in a key event to check to see if it can be converted
	//	to a command. Returns true if a command handled the key.
	//--------------------------------------------------------------------
	bool ProcessKeyEvent(const QKeyEvent& i_Event);

	//--------------------------------------------------------------------
	// Add a widget to list of widgets that want to handle
	//	hot key events (key presses in it or its children).
	//--------------------------------------------------------------------
	void WindowWantsHotKeys(QWidget* i_pWindow);

	//--------------------------------------------------------------------
	// Return true if widget wants to receive hot keys.
	//--------------------------------------------------------------------
	bool WantsHotKeys(QWidget* i_pWindow);

	//--------------------------------------------------------------------
	// Return true if there are any widgets that want to process hot keys.
	//--------------------------------------------------------------------
	bool ShouldProcessHotKeys();
}

#endif // USE_QT
