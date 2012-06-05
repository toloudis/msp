/****************************************************************************\
**	tqcTextBox.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcTextBox.hpp"

#ifdef QT_FINISH_PORT

BEGIN_EVENT_TABLE(tqcTextBox, wxTextCtrl)
    EVT_TEXT_ENTER(wxID_ANY, tqcTextBox::OnTextChange)
	EVT_KILL_FOCUS(tqcTextBox::OnTextLeave)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcTextBox::tqcTextBox(QWidget* i_pParent, 
					   QWidgetID i_Id, 
					   const wxString& i_Value,
					   bool i_bMultiline,
					   bool i_bMultilineFixed,
					   const wxSize& i_Size)
:	wxTextCtrl(i_pParent, i_Id, i_Value, wxDefaultPosition,
			   i_bMultilineFixed ?  wxSize(123,123) : i_Size, 
			   i_bMultiline ? wxTE_MULTILINE : wxTE_PROCESS_ENTER),
	m_LastValue(i_Value),
	m_bMultiline(i_bMultiline)
{		
}

//--------------------------------------------------------------------
// Override SetValue so that we can track the current value
//	in order to know when it changes.
//--------------------------------------------------------------------
//virtual 
void tqcTextBox::SetValue(const wxString& i_Value)
{
	m_LastValue = i_Value;
	//bga - experimenting
	wxTextCtrl::SetValue(i_Value);
	//wxTextCtrl::Clear();
	//wxTextCtrl::AppendText(i_Value);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcTextBox::OnTextChange(wxCommandEvent& i_Event)
{
	text_changed();

	// After Enter is pressed, select the full text 
	// in order to make it easier to try out new values
	if ( !m_bMultiline ) 
	{
		this->SetSelection(-1,-1);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcTextBox::OnTextLeave(wxFocusEvent& i_Event)
{
	text_changed();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcTextBox::text_changed()
{
	wxString text = this->GetValue();
	// We need to store the last value to restore when used in the filePicker and is partially deleted.
	m_PrevValue = m_LastValue;
	if (text != m_LastValue)
	{
		m_LastValue = text;

		// trigger the callback
		wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
		this->ProcessCommand(changed_event);
	}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
wxString tqcTextBox::last_value()
{
	return m_PrevValue;
}

#endif // USE_QT
