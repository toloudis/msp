/****************************************************************************\
**	tqcFloatEdit.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcFloatEdit.hpp"

#include "ToolUIQt/tqc/tqcEvent.hpp"

#include <sstream>

#ifdef USE_QT
#ifdef QT_FINISH_PORT
BEGIN_EVENT_TABLE(tqcFloatEdit, wxTextCtrl)
    EVT_TEXT_ENTER(wxID_ANY, tqcFloatEdit::OnTextChange)
	EVT_KILL_FOCUS(tqcFloatEdit::OnTextLeave)
END_EVENT_TABLE()
#endif

//----------------------------------------------------------------------------
// The wxTE_PROCESS_ENTER window style is necessary in order 
//	to trap for the ENTER pressed event
//----------------------------------------------------------------------------
tqcFloatEdit::tqcFloatEdit(QWidget* i_pParent)
#ifdef QT_FINISH_PORT
						   QWidgetID i_Id,
						   const wxPoint& i_Pos,
						   const wxSize& i_Size)
#endif
:	QLineEdit(i_pParent), //, i_Id, L"0", i_Pos, i_Size, wxTE_PROCESS_ENTER),
	m_FloatValue(0),
	m_DecimalPlaces(-1)
{
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const float tqcFloatEdit::GetFloatValue() const
{
	return m_FloatValue;
}
void tqcFloatEdit::SetFloatValue(const float i_Value)
{
	m_FloatValue = i_Value;

	std::wostringstream str;
	// Default (-1) uses 6 digits of precision in natural stream method.
	// If m_DecimalPlaces has been set, then switch to fixed format and
	// limit the number of decimal places.
	if (m_DecimalPlaces >= 0)
	{
		str.setf(std::ios_base::fixed, std::ios_base::floatfield);
		str.precision(m_DecimalPlaces);
	}
	str << m_FloatValue;
#ifdef QT_FINISH_PORT
	this->ChangeValue(str.str());
#endif
}

//--------------------------------------------------------------------
//	DecimalPlaces
//--------------------------------------------------------------------
const short tqcFloatEdit::GetDecimalPlaces() const
{
	return m_DecimalPlaces;
}
void tqcFloatEdit::SetDecimalPlaces(const short i_DecimalPlaces)
{
	m_DecimalPlaces = i_DecimalPlaces;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#ifdef QT_FINISH_PORT
void tqcFloatEdit::OnTextChange(wxCommandEvent& i_Event)
{
	text_changed();

	// After Enter is pressed, select the full text 
	// in order to make it easier to try out new values
	this->SetSelection(-1,-1);
}
#endif

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#ifdef QT_FINISH_PORT
void tqcFloatEdit::OnTextLeave(wxFocusEvent& i_Event)
{
	text_changed();
}
#endif

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcFloatEdit::text_changed()
{
	float old_value = m_FloatValue;

#ifdef QT_FINISH_PORT
	std::wstring text = this->GetValue();
	std::wistringstream str(text);
	str >> m_FloatValue;

	if (m_FloatValue != old_value)
	{
		// trigger the callback
		wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
		this->ProcessCommand(changed_event);
	}
#endif
}

#endif // USE_QT
