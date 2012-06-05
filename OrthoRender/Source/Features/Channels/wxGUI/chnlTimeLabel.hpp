/****************************************************************************\
**	chnlTimeLabel.hpp
**
**		Custom control to display time marks on a timeline
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TIMELABEL_HPP
#error chnlTimeLabel.hpp multiply included
#endif
#define CHNL_TIMELABEL_HPP

#ifndef CHNL_TIMECOMMON_HPP
#include "Features/Channels/wxGUI/chnlTimeCommon.hpp"
#endif 
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class chnlTimeLabel : public wxWindow, public chnlTimeCommon
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlTimeLabel(wxWindow* i_pParent);

		//--------------------------------------------------------------------
		// SetFramesPerSecond
		//--------------------------------------------------------------------
		void SetFramesPerSecond(int i_FPS);
		int GetFramesPerSecond() const;

		//--------------------------------------------------------------------
		// SetDisplayFrames - display numbers as seconds or frames
		//--------------------------------------------------------------------
		void SetDisplayFrames(bool i_bDisplayFrames);
		bool GetDisplayFrames() const;

		//--------------------------------------------------------------------
		// SetDisplayMinutes - display numbers as seconds or minutes:seconds
		//--------------------------------------------------------------------
		void SetDisplayMinutes(bool i_bDisplayMinutes);
		bool GetDisplayMinutes() const;

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

		int m_FramesPerSecond;
		bool m_bDisplayFrames, m_bDisplayMinutes;

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
