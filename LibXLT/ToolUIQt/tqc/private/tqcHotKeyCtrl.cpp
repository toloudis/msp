/****************************************************************************\
**	tqcHotKeyCtrl.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcHotKeyCtrl.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "ToolUIQt/tqt/tqtMessaging.hpp"


#ifdef QT_FINISH_PORT

BEGIN_EVENT_TABLE(tqcHotKeyCtrl, wxTextCtrl)
    EVT_KEY_DOWN(tqcHotKeyCtrl::OnKeyDown)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcHotKeyCtrl::tqcHotKeyCtrl(QWidget* i_pParent, 
					   QWidgetID i_Id)
:	wxTextCtrl(i_pParent, i_Id, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_READONLY),
	m_LastValue("")
{		
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcHotKeyCtrl::OnKeyDown(wxKeyEvent& i_Event)
{
	bool bHaveHotKey = false;
	std::string hot_key_str;
	if (i_Event.GetKeyCode() == WXK_BACK)
	{
		// Backspace clears the control
		hot_key_str = "";
		bHaveHotKey = true;
	}
	else if (tqtMessaging::ConvertKeyEventToString(i_Event, hot_key_str))
	{
		bHaveHotKey = true;
	}

	if (bHaveHotKey)
	{		
		wxString hot_key_value(hot_key_str.c_str(), wxConvUTF8);
		wxString prev_key_value(this->GetValue(), wxConvUTF8);
		if (hot_key_value != this->GetValue())
		{
			cmaCommandMgr::ResetValidHotKey();
			this->SetValue( hot_key_value );

			// trigger the callback
			wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
			this->ProcessCommand(changed_event);
			//if the hot key is not valid, the text field will be empty
			if(!cmaCommandMgr::IsValidHotKey())
			{
				this->SetValue(prev_key_value);
				this->ProcessCommand(changed_event);
			}
		}
	}
}

#endif // USE_QT
