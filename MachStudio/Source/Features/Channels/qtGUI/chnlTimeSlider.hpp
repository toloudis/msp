/****************************************************************************\
**	chnlTimeSlider.hpp
**
**		Current time indicator above the timeline, in Qt.  The value
**	changed callback is called when the user changes the current time.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TIMESLIDER_HPP
#error chnlTimeSlider.hpp multiply included
#endif
#define CHNL_TIMESLIDER_HPP

#ifndef CHNL_TIMECOMMON_HPP
#include "Features/Channels/qtGUI/chnlTimeCommon.hpp"
#endif
#ifndef TQC_EVENT_HPP
#include "ToolUIQt/tqc/tqcEvent.hpp"
#endif

#include <set>

#ifdef USE_QT

//============================================================================
//============================================================================
class chnlTimeSlider : public QWidget, public chnlTimeCommon, public tqcValueChangedNotifier
{
	public:
		chnlTimeSlider(QWidget* i_pParent);

		//--------------------------------------------------------------------
		// CurrentTime
		//--------------------------------------------------------------------
		void SetCurrentTime(float i_Time);
		float GetCurrentTime() const;

		//--------------------------------------------------------------------
		// Get the horizontal position of the current time
		//--------------------------------------------------------------------
		int GetCurrentTimePosition();

		//--------------------------------------------------------------------
		//	Get the time based on the horizontal position
		//--------------------------------------------------------------------
		float GetTimeAtPosition(int i_ScreenXPos);

		//--------------------------------------------------------------------
		// Clear time ticks
		//--------------------------------------------------------------------
		void ClearTimeTicks();

		//--------------------------------------------------------------------
		// Add a time tick
		//--------------------------------------------------------------------
		void AddTimeTick(float i_Time);

	protected:
		void paintEvent(QPaintEvent* i_pEvent) override;
		void mousePressEvent(QMouseEvent* i_pEvent) override;
		void mouseMoveEvent(QMouseEvent* i_pEvent) override;
		void mouseReleaseEvent(QMouseEvent* i_pEvent) override;

	private:
		//--------------------------------------------------------------------
		// Virtual function called when time range or scale has changed
		//--------------------------------------------------------------------
		virtual void update_size();

		void compute_size();
		void stop_scrubbing();
		void handle_mouse_event(int i_X);

		float m_CurrentTime;
		std::set<float> m_TimeTicks;
		bool m_bScrubbing;
};

#endif // USE_QT
