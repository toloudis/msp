/****************************************************************************\
**	chnlTimeLabel.hpp
**
**		Time labels and hash marks above the timeline, in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TIMELABEL_HPP
#error chnlTimeLabel.hpp multiply included
#endif
#define CHNL_TIMELABEL_HPP

#ifndef CHNL_TIMECOMMON_HPP
#include "Features/Channels/qtGUI/chnlTimeCommon.hpp"
#endif
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

//============================================================================
//============================================================================
class chnlTimeLabel : public QWidget, public chnlTimeCommon
{
	public:
		chnlTimeLabel(QWidget* i_pParent);

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

	protected:
		void paintEvent(QPaintEvent* i_pEvent) override;

	private:
		//--------------------------------------------------------------------
		// Virtual function called when time range or scale has changed
		//--------------------------------------------------------------------
		virtual void update_size();

		void compute_size();

		int m_FramesPerSecond;
		bool m_bDisplayFrames, m_bDisplayMinutes;
};

#endif // USE_QT
