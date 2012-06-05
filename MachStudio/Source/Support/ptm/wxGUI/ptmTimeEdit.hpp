/****************************************************************************\
**	ptmTimeEdit.hpp
**
**		Custom text control for editing time values.
**
**	StudioGPU
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
#ifndef MA_TIME_HPP
#include "Core/Ma/maTime.hpp"
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
		const maTime& GetTimeValue() const;
		void SetTimeValue(const maTime& i_Value);

//============================================================================
// TimeInterest callbacks
//============================================================================

		//--------------------------------------------------------------------
		//	TimeChanged - timeline current time has changed
		//--------------------------------------------------------------------
		virtual void TimeChanged( const maTime& i_Time ) {}

		//--------------------------------------------------------------------
		//	TimeRangeChanged - timeline maximum time has changed
		//--------------------------------------------------------------------
		virtual void TimeRangeChanged( const maTime& i_MinTime, const maTime& i_MaxTime ) {}

		//--------------------------------------------------------------------
		//	TimeFormatChanged - timeline time display format has changed
		//--------------------------------------------------------------------
		virtual void TimeFormatChanged( int i_TimeFormat );

		//--------------------------------------------------------------------
		//	FrameRateChanged - timeline frame rate (frames per second) changed
		//--------------------------------------------------------------------
		virtual void FrameRateChanged( float i_FrameRate );

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange(wxCommandEvent& i_Event);
		void OnTextLeave(wxFocusEvent& i_Event);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void text_changed();

		maTime m_TimeValue;

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
