/*****************************************************************************
**  appCharEvent.hpp
**
**      appCharEvent is Terawatt event type representing a text character
**	entered with the keyboard.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_CHAREVENT_HPP
#error appCharEvent.hpp multiply included
#endif
#define APP_CHAREVENT_HPP

#ifndef APP_EVENT_HPP
#include "Core/app/appEvent.hpp"
#endif

#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//============================================================================
class appCharEvent : public appEvent
{
	public:

		// TODO: Fix app char event
		// When the application receives the WM_KEYDOWN event the
		// key that was pressed does not specify whether it was the left 
		// or right version for ALT, SHIFT, and CTRL
		// The problem is in the appApplicationPACWin
		// I believe you need to call MapVirtualKey or maybe there is a bit
		// in the lparam of the WM_KEYDOWN event that can be used to 
		// differentiate between left and right.  

		//--------------------------------------------------------------------
		//	enums
		//--------------------------------------------------------------------
		enum SpecialKeys
		{
			e_F1 = 0xE000,
			e_F2,
			e_F3,
			e_F4,
			e_F5,
			e_F6,
			e_F7,
			e_F8,
			e_F9,
			e_F10,
			e_F11,
			e_F12,
			e_PRINT,
			e_SCROLL,
			e_PAUSE,
			e_LCTRL,
			e_LWIN,
			e_LALT,
			e_RALT,
			e_RWIN,
			e_RCTRL,
			e_INSERT,
			e_HOME,
			e_PAGEUP,
			e_DELETE,
			e_END,
			e_PAGEDOWN,
			e_UP,
			e_DOWN,
			e_LEFT,
			e_RIGHT,
			e_NUMLOCK,

			//these have actual values associated in unicode, it is a convenience to give them those values here
			e_BACKSPACE = 8,
			e_TAB = 9,	// mathces VK_TAB in appApplicationPACWin.cpp
			e_ENTER = 13,
			e_NUMPADENTER = 13,
			e_ESC = 27,
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		appCharEvent(itString::CharType i_Char);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~appCharEvent();

		//--------------------------------------------------------------------
		//	GetChar returns the relevant character.
		//--------------------------------------------------------------------
		itString::CharType GetChar() const;

	private:

		itString::CharType m_Char;
};

