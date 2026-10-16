/*****************************************************************************
**	qtResolvePathDialogBase.h
**
**		Layout of the dialog asking what to do about a missing file
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifndef __qtResolvePathDialogBase__
#define __qtResolvePathDialogBase__

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

///////////////////////////////////////////////////////////////////////////////
/// Class qtResolvePathDialogBase
///////////////////////////////////////////////////////////////////////////////
class qtResolvePathDialogBase : public QDialog
{
	protected:
		QLabel* m_staticText1;
		QLabel* m_staticText_Filename;
		QPushButton* m_button_Locate;
		QPushButton* m_button_Retry;
		QPushButton* m_button_Skip;
		QPushButton* m_button_SkipAll;
		QPushButton* m_button_Abort;

		// Virtual event handlers, overide them in your derived class
		virtual void buttonLocateClick() {}
		virtual void buttonRetryClick() {}
		virtual void buttonSkipClick() {}
		virtual void buttonSkipAllClick() {}
		virtual void buttonAbortClick() {}

	public:
		qtResolvePathDialogBase( QWidget* parent, const QString& title = "Resolve Path" );
		~qtResolvePathDialogBase();
};

#endif // USE_QT

#endif //__qtResolvePathDialogBase__
