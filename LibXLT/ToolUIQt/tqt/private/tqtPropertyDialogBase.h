/*****************************************************************************
**	tqtPropertyDialogBase.h
**
**		Layout of the property dialog: a message, a scrolling panel of
**	property controls, and OK / Cancel buttons.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifndef __tqtPropertyDialogBase__
#define __tqtPropertyDialogBase__

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

///////////////////////////////////////////////////////////////////////////////
/// Class tqtPropertyDialogBase
///////////////////////////////////////////////////////////////////////////////
class tqtPropertyDialogBase : public QDialog
{
	protected:
		QLabel* m_staticText_Message;
		QScrollArea* m_scroll_Properties;
		QWidget* m_panel_Properties;	// holds the property controls
		QDialogButtonBox* m_sdbSizer1;

	public:
		tqtPropertyDialogBase( QWidget* parent, const QString& title = QString() );
		~tqtPropertyDialogBase();
};

#endif // USE_QT

#endif //__tqtPropertyDialogBase__
