/****************************************************************************\
**	twcHotKeyCtrl.hpp
**
**		Textbox that handles key press events, mapping them to
**	hotkey strings.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_HOTKEYCTRL_HPP
#error twcHotKeyCtrl.hpp multiply included
#endif
#define TWC_HOTKEYCTRL_HPP

#ifndef TWC_EVENT_HPP
#include "ToolUIWx/twc/twcEvent.hpp"
#endif


#ifdef USE_WXWIDGETS

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed aftere ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class twcHotKeyCtrl : public wxTextCtrl
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcHotKeyCtrl(wxWindow* i_pParent, 
				   wxWindowID i_Id = wxID_ANY);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnKeyDown(wxKeyEvent& i_Event);

		std::string m_LastValue;

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
