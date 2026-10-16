/*****************************************************************************
**	tqtPropertyDialogBase.cpp
**
**		see .h
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

#include "tqtPropertyDialogBase.h"

///////////////////////////////////////////////////////////////////////////

tqtPropertyDialogBase::tqtPropertyDialogBase( QWidget* parent, const QString& title )
: QDialog( parent )
{
	this->setWindowTitle(title);
	this->setSizeGripEnabled(true);

	QVBoxLayout* bSizer1 = new QVBoxLayout(this);

	m_staticText_Message = new QLabel( this );
	m_staticText_Message->setWordWrap(true);
	m_staticText_Message->hide();
	bSizer1->addWidget( m_staticText_Message );

	m_scroll_Properties = new QScrollArea( this );
	m_scroll_Properties->setWidgetResizable(true);
	m_scroll_Properties->setFrameShape(QFrame::NoFrame);
	m_panel_Properties = new QWidget;
	m_scroll_Properties->setWidget(m_panel_Properties);
	bSizer1->addWidget( m_scroll_Properties, 1 );

	m_sdbSizer1 = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this );
	QObject::connect(m_sdbSizer1, &QDialogButtonBox::accepted, this, &QDialog::accept);
	QObject::connect(m_sdbSizer1, &QDialogButtonBox::rejected, this, &QDialog::reject);
	bSizer1->addWidget( m_sdbSizer1, 0, Qt::AlignCenter );
}

tqtPropertyDialogBase::~tqtPropertyDialogBase()
{
}

#endif
