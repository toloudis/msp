/*****************************************************************************
**  qtMainDropTarget.hpp
**
**     Handles files dropped on a render panel: scenes are opened,
**	materials are pasted where they are dropped, and characters are added.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef QT_MAINDROPTARGET_HPP
#error qtMainDropTarget.hpp multiply included
#endif
#define QT_MAINDROPTARGET_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

//============================================================================
//============================================================================
class tma3dRenderView;

#ifdef USE_QT

//============================================================================
// Class qtMainDropTarget - watches the drag and drop events of a widget.
//	It is owned by (and deleted with) the widget.
//============================================================================
class qtMainDropTarget : public QObject
{
	public:
		//--------------------------------------------------------------------
		// Accept file drops on the widget for the render view
		//--------------------------------------------------------------------
		qtMainDropTarget(QWidget* i_pWidget, tma3dRenderView *i_pRenderView);
		~qtMainDropTarget();

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual bool eventFilter(QObject* i_pWatched, QEvent* i_pEvent);

	private:
		//--------------------------------------------------------------------
		// x, y are in physical pixels of the render panel
		//--------------------------------------------------------------------
		bool OnDropFiles(int x, int y, const QStringList& filenames);

		QWidget *m_pWidget;
		tma3dRenderView *m_pRenderView;
};

#endif // USE_QT
