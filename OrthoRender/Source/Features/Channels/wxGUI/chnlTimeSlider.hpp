/****************************************************************************\
**	chnlTimeSlider.hpp
**
**		Custom control to display and control current time
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TIMESLIDER_HPP
#error chnlTimeSlider.hpp multiply included
#endif
#define CHNL_TIMESLIDER_HPP

#ifndef CHNL_TIMECOMMON_HPP
#include "Features/Channels/wxGUI/chnlTimeCommon.hpp"
#endif 
#ifndef TWC_EVENT_HPP
#include "ToolUIWx/twc/twcEvent.hpp"
#endif

#include <set>

#ifdef USE_WXWIDGETS

//============================================================================
// wxEVT_VALUE_CHANGED event is thrown when the current time is changed
//============================================================================

//============================================================================
//============================================================================
class chnlTimeSlider : public wxControl, public chnlTimeCommon
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlTimeSlider(wxWindow* i_pParent);

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

	private:
		//--------------------------------------------------------------------
		// Virtual function called when time range or scale has changed
		//--------------------------------------------------------------------
		virtual void update_size();

		//--------------------------------------------------------------------
		// private functions
		//--------------------------------------------------------------------
		void compute_size();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnPaint(wxPaintEvent &i_Event);
		void OnMouseUp(wxMouseEvent &i_Event);
		void OnMouseDown(wxMouseEvent &i_Event);
		void OnMouseMove(wxMouseEvent &i_Event);
		void handle_mouse_event(wxMouseEvent &i_Event);

		float m_CurrentTime;
		std::set<float> m_TimeTicks;

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
