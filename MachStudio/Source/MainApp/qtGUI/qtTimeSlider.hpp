/*****************************************************************************
**  qtTimeSlider.hpp
**
**     Pane at the bottom of the main window with the time line slider
**	and the current, minimum and maximum times.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef QT_TIMESLIDER_HPP
#error qtTimeSlider.hpp multiply included
#endif
#define QT_TIMESLIDER_HPP

#ifndef TQC_RANGEDFLOAT_HPP
#include "ToolUIQt/tqc/tqcRangedFloat.hpp"
#endif

#ifndef TMLN_TIMEINTEREST_HPP
#include "Support/tmln/tmlnTimeInterest.hpp"
#endif

#ifdef USE_QT

//============================================================================
//	Forward Declarations
//============================================================================
class ptmTimeEdit;


//============================================================================
// Class qtTimeSlider
//============================================================================
class qtTimeSlider : public QWidget, public tmlnTimeInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		qtTimeSlider( QWidget* parent );
		~qtTimeSlider();

//============================================================================
// TimeInterest callbacks
//============================================================================

		//--------------------------------------------------------------------
		//	TimeChanged - timeline current time has changed
		//--------------------------------------------------------------------
		virtual void TimeChanged( const maTime& i_Time );

		//--------------------------------------------------------------------
		//	TimeRangeChanged - timeline maximum time has changed
		//--------------------------------------------------------------------
		virtual void TimeRangeChanged( const maTime& i_MinTime, const maTime& i_MaxTime );

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
		// event callbacks
		//--------------------------------------------------------------------
		void SliderThumbDown();
		void SliderThumbUp();
		void SliderChanged();
		void CurrentTimeChanged();
		void MaxTimeChanged();
		void MinTimeChanged();

		tqcRangedFloat* m_slider1;
		ptmTimeEdit* m_floatEdit_CurrentTime;
		ptmTimeEdit* m_floatEdit_MaxTime;
		ptmTimeEdit* m_floatEdit_MinTime;
};

#endif // USE_QT
