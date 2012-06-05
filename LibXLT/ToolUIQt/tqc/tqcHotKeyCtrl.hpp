/****************************************************************************\
**	tqcHotKeyCtrl.hpp
**
**		Textbox that handles key press events, mapping them to
**	hotkey strings.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_HOTKEYCTRL_HPP
#error tqcHotKeyCtrl.hpp multiply included
#endif
#define TQC_HOTKEYCTRL_HPP

#ifndef TQC_EVENT_HPP
#include "ToolUIQt/tqc/tqcEvent.hpp"
#endif


#ifdef QT_FINISH_PORT

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed aftere ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class tqcHotKeyCtrl : public wxTextCtrl
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcHotKeyCtrl(QWidget* i_pParent, 
				   QWidgetID i_Id = wxID_ANY);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnKeyDown(wxKeyEvent& i_Event);

		std::string m_LastValue;

    DECLARE_EVENT_TABLE()
};

#endif // USE_QT
