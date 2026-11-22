/****************************************************************************\
**	ptmTimeEdit.hpp
**
**	Text box for editing a time value in the timeline's current time
**	format. 
**	
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef PTM_TIMEEDIT_HPP
#error ptmTimeEdit.hpp multiply included
#endif
#define PTM_TIMEEDIT_HPP

#ifndef TQC_EVENT_HPP
#include "ToolUIQt/tqc/tqcEvent.hpp"
#endif

#ifndef TMLN_TIMEINTEREST_HPP
#include "Support/tmln/tmlnTimeInterest.hpp"
#endif 

#ifndef MA_TIME_HPP
#include "Core/Ma/maTime.hpp"
#endif 

#ifdef USE_QT

//============================================================================
// Value changed callback (tqcEvent.hpp): this control calls it when the
//	value in the control has changed after ENTER is pressed, or the
//	focus leaves the text box.
//============================================================================

//============================================================================
//============================================================================
class ptmTimeEdit : public QLineEdit, public tqcValueChangedNotifier, public tmlnTimeInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ptmTimeEdit(QWidget* i_pParent);

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
		void text_changed();

		maTime m_TimeValue;
};

#endif // USE_QT
