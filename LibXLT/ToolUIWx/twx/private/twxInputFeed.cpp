/****************************************************************************\
**	twxInputFeed.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twx/twxInputFeed.hpp"

#include "Input/in/inDeviceMgr.hpp"

#ifdef USE_WXWIDGETS

namespace twxInputFeed
{
	namespace
	{
		bool l_bFeeding = false;

		//--------------------------------------------------------------------
		// Passes key events to the keyboard
		//--------------------------------------------------------------------
		void feed_key(wxKeyEvent& i_Event, bool i_bDown)
		{
			inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
			if (!pKeyboard)
				return;

#ifdef __WXMSW__
			// The raw flags are the key message's lParam: the scan code is
			// in bits 16-23, and bit 24 is set for extended keys
			const wxUint32 flags = i_Event.GetRawKeyFlags();
			const unsigned int scan_code = (flags >> 16) & 0xFF;
			const unsigned int extended = ((flags >> 24) & 1) ? 0x100 : 0;
			if (scan_code != 0)
				pKeyboard->SetNativeKeyState(scan_code | extended, i_bDown);
#else
#error twxInputFeed has no key scan codes for this platform
#endif
		}

		//--------------------------------------------------------------------
		// Marks every key as up, for when the key-up events will go to
		// another window or application
		//--------------------------------------------------------------------
		void release_keys()
		{
			if (inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard())
				pKeyboard->ReleaseAll();
		}

		//--------------------------------------------------------------------
		// Tells the mouse about a button press
		//--------------------------------------------------------------------
		void feed_button(int i_Button)
		{
			if (inMouse* pMouse = inDeviceMgr::GetMouse())
				pMouse->NotifyButtonPressed(i_Button);
		}

		// ----------------------------------------------------------------------------
		// Reads the cursor and buttons from wxWidgets
		// ----------------------------------------------------------------------------
		class MouseSource : public inMouseSource
		{
		public:
			virtual bool Sample(int& o_ScreenX, int& o_ScreenY, unsigned int& o_Buttons)
			{
				if (!wxTheApp || !wxTheApp->IsActive())
					return false;

				const wxMouseState state = wxGetMouseState();
				o_ScreenX = state.GetX();
				o_ScreenY = state.GetY();

				o_Buttons = 0;
				if (state.LeftIsDown())		o_Buttons |= 1u << inMouse::e_LEFT;
				if (state.RightIsDown())	o_Buttons |= 1u << inMouse::e_RIGHT;
				if (state.MiddleIsDown())	o_Buttons |= 1u << inMouse::e_MIDDLE;
				if (state.Aux1IsDown())		o_Buttons |= 1u << inMouse::e_EXTRA1;
				if (state.Aux2IsDown())		o_Buttons |= 1u << inMouse::e_EXTRA2;
				return true;
			}
		};

		MouseSource* l_pMouseSource = NULL;

	} // local namespace

	//--------------------------------------------------------------------
	// Init starts feeding the input package
	//--------------------------------------------------------------------
	void Init()
	{
		if (l_bFeeding)
			return;
		l_bFeeding = true;

		l_pMouseSource = new MouseSource;
		if (inMouse* pMouse = inDeviceMgr::GetMouse())
			pMouse->SetSource(l_pMouseSource);
	}

	//--------------------------------------------------------------------
	// CleanUp stops feeding the input package
	//--------------------------------------------------------------------
	void CleanUp()
	{
		if (!l_bFeeding)
			return;
		l_bFeeding = false;

		if (inMouse* pMouse = inDeviceMgr::GetMouse())
			pMouse->SetSource(NULL);
		delete l_pMouseSource;
		l_pMouseSource = NULL;
	}

	//--------------------------------------------------------------------
	// FilterEvent looks at every event in the application
	//--------------------------------------------------------------------
	void FilterEvent(wxEvent& i_Event)
	{
		if (!l_bFeeding)
			return;

		const wxEventType type = i_Event.GetEventType();
		if (type == wxEVT_KEY_DOWN)
			feed_key(static_cast<wxKeyEvent&>(i_Event), true);
		else if (type == wxEVT_KEY_UP)
			feed_key(static_cast<wxKeyEvent&>(i_Event), false);
		else if (type == wxEVT_LEFT_DOWN || type == wxEVT_LEFT_DCLICK)
			feed_button(inMouse::e_LEFT);
		else if (type == wxEVT_RIGHT_DOWN || type == wxEVT_RIGHT_DCLICK)
			feed_button(inMouse::e_RIGHT);
		else if (type == wxEVT_MIDDLE_DOWN || type == wxEVT_MIDDLE_DCLICK)
			feed_button(inMouse::e_MIDDLE);
		else if (type == wxEVT_AUX1_DOWN || type == wxEVT_AUX1_DCLICK)
			feed_button(inMouse::e_EXTRA1);
		else if (type == wxEVT_AUX2_DOWN || type == wxEVT_AUX2_DCLICK)
			feed_button(inMouse::e_EXTRA2);
		else if (type == wxEVT_ACTIVATE || type == wxEVT_ACTIVATE_APP)
		{
			if (!static_cast<wxActivateEvent&>(i_Event).GetActive())
				release_keys();

			// So motion made in another application doesn't arrive all at once
			if (type == wxEVT_ACTIVATE_APP)
			{
				if (inMouse* pMouse = inDeviceMgr::GetMouse())
					pMouse->ResetMotion();
			}
		}
	}
}

#endif // USE_WXWIDGETS
