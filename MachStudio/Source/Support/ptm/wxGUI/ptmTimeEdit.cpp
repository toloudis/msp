/****************************************************************************\
**	ptmTimeEdit.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/ptm/wxGUI/ptmTimeEdit.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Tool/gui/guiMessageBox.hpp"

#include <sstream>


#ifdef USE_WXWIDGETS
//============================================================================
//============================================================================
BEGIN_EVENT_TABLE(ptmTimeEdit, wxTextCtrl)
    EVT_TEXT_ENTER(wxID_ANY, ptmTimeEdit::OnTextChange)
	EVT_KILL_FOCUS(ptmTimeEdit::OnTextLeave)
END_EVENT_TABLE()


//----------------------------------------------------------------------------
// The wxTE_PROCESS_ENTER window style is necessary in order 
//	to trap for the ENTER pressed event
//----------------------------------------------------------------------------
ptmTimeEdit::ptmTimeEdit(wxWindow* i_pParent, 
						   wxWindowID i_Id,
						   const wxPoint& i_Pos,
						   const wxSize& i_Size,
						   long i_Style)
:	wxTextCtrl(i_pParent, i_Id, L"0", i_Pos, i_Size, (i_Style | wxTE_PROCESS_ENTER))
{		
	// Add this control as a time interest
	tmlnTimeLine::AddTimeInterest(this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptmTimeEdit::~ptmTimeEdit()
{
	// Unregister time interest
	tmlnTimeLine::RemoveTimeInterest(this);
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maTime& ptmTimeEdit::GetTimeValue() const
{
	return m_TimeValue;
}
void ptmTimeEdit::SetTimeValue(const maTime& i_Value)
{
	m_TimeValue = i_Value;

	std::string time_string;
	tmlnTimeUtil::GetTimeString(m_TimeValue, time_string);
	this->ChangeValue(wxString(time_string.c_str(), wxConvUTF8));
}

//--------------------------------------------------------------------
//	TimeFormatChanged - timeline time display format has changed
//--------------------------------------------------------------------
//virtual 
void ptmTimeEdit::TimeFormatChanged( int i_TimeFormat )
{
	// Force through a "Set" again in order to alter text in the text box
	// to fit the new time format
	this->SetTimeValue(m_TimeValue);
}

//--------------------------------------------------------------------
//	FrameRateChanged - timeline frame rate (frames per second) changed
//--------------------------------------------------------------------
//virtual 
void ptmTimeEdit::FrameRateChanged( float i_FrameRate )
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ptmTimeEdit::OnTextChange(wxCommandEvent& i_Event)
{
	text_changed();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ptmTimeEdit::OnTextLeave(wxFocusEvent& i_Event)
{
	text_changed();
    i_Event.Skip();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ptmTimeEdit::text_changed()
{
	std::string text = this->GetValue().utf8_str();
	if (!text.empty())
	{
		maTime new_value = m_TimeValue;
		if (tmlnTimeUtil::ParseTimeString(text, new_value))
		{
			if (m_TimeValue != new_value)
			{
				m_TimeValue = new_value;

				// trigger the callback
				wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
				this->ProcessCommand(changed_event);
			}
		}
		else
		{
			// Error parsing time string, display error message
			std::string message("Please enter time in the current time format: ");
			std::string time_format = tmlnTimeUtil::GetTimeFormatString();
			message += time_format;
			//DBG_LOG2("Time parsing error, %s not in format %s", text.c_str(), time_format.c_str());
			guiMessageBox::Show(message.c_str(), "Time entry parsing");
		}
	}
}

#endif // USE_WXWIDGETS
