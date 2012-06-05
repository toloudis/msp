/****************************************************************************\
**	ptmTimeEdit.hpp
**
**		Custom text control for editing time values.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PTM_TIMEEDIT_HPP
#error ptmTimeEdit.hpp multiply included
#endif
#define PTM_TIMEEDIT_HPP

#ifndef TWC_EVENT_HPP
#include "ToolUIWx/twc/twcEvent.hpp"
#endif
#ifndef TMLN_TIMEINTEREST_HPP
#include "Support/tmln/tmlnTimeInterest.hpp"
#endif 

#ifdef USE_WXWIDGETS

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed after ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class ptmTimeEdit : public wxTextCtrl, public tmlnTimeInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ptmTimeEdit(wxWindow* i_pParent, 
					 wxWindowID i_Id = wxID_ANY,
					 const wxPoint& i_Pos = wxDefaultPosition, 
					 const wxSize& i_Size = wxDefaultSize,
					 long i_Style = 0);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~ptmTimeEdit();

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const float GetTimeValue() const;
		void SetTimeValue(const float i_Value);

//============================================================================
// TimeInterest callbacks
//============================================================================

		//--------------------------------------------------------------------
		//	TimeChanged - timeline current time has changed
		//--------------------------------------------------------------------
		virtual void TimeChanged( float i_Time ) {}

		//--------------------------------------------------------------------
		//	TimeRangeChanged - timeline maximum time has changed
		//--------------------------------------------------------------------
		virtual void TimeRangeChanged( float i_MinTime, float i_MaxTime ) {}

		//--------------------------------------------------------------------
		//	TimeFormatChanged - timeline time display format has changed
		//--------------------------------------------------------------------
		virtual void TimeFormatChanged( int i_TimeFormat );

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange(wxCommandEvent& i_Event);
		void OnTextLeave(wxFocusEvent& i_Event);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void text_changed();

		float m_TimeValue;

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
