/****************************************************************************\
**	chnlTimelinePanel.hpp
**
**		Panel holding the channel controls. It draws the timeline guides
**	behind the channels and passes its own mouse events (the ones that are
**	not on a channel) to the given callbacks.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TIMELINEPANEL_HPP
#error chnlTimelinePanel.hpp multiply included
#endif
#define CHNL_TIMELINEPANEL_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <functional>

#ifdef USE_QT

//============================================================================
//============================================================================
class chnlTimelinePanel : public QWidget
{
	public:
		chnlTimelinePanel(QWidget* i_pParent);

		typedef std::function<void (QMouseEvent*)> MouseFunction;
		void SetMouseCallbacks(MouseFunction i_Down, MouseFunction i_Move, MouseFunction i_Up);

	protected:
		void paintEvent(QPaintEvent* i_pEvent) override;
		void mousePressEvent(QMouseEvent* i_pEvent) override;
		void mouseMoveEvent(QMouseEvent* i_pEvent) override;
		void mouseReleaseEvent(QMouseEvent* i_pEvent) override;

	private:
		MouseFunction m_MouseDown, m_MouseMove, m_MouseUp;
};

#endif // USE_QT
