/*****************************************************************************
**	qtResolvePathDialogBase.cpp
**
**		see .h
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/qtGUI/qtResolvePathDialogBase.h"

#ifdef USE_QT

///////////////////////////////////////////////////////////////////////////

qtResolvePathDialogBase::qtResolvePathDialogBase( QWidget* parent, const QString& title )
: QDialog( parent )
{
	this->setWindowTitle(title);

	QVBoxLayout* bSizer1 = new QVBoxLayout(this);

	m_staticText1 = new QLabel( "File not found:", this );
	bSizer1->addWidget( m_staticText1 );

	m_staticText_Filename = new QLabel( "Message goes here", this );
	m_staticText_Filename->setTextInteractionFlags(Qt::TextSelectableByMouse);
	bSizer1->addWidget( m_staticText_Filename, 1 );

	QHBoxLayout* bSizer2 = new QHBoxLayout;
	m_button_Locate = new QPushButton( "Locate...", this );
	bSizer2->addWidget( m_button_Locate );
	m_button_Retry = new QPushButton( "Retry", this );
	bSizer2->addWidget( m_button_Retry );
	m_button_Skip = new QPushButton( "Skip", this );
	bSizer2->addWidget( m_button_Skip );
	m_button_SkipAll = new QPushButton( "Skip All", this );
	bSizer2->addWidget( m_button_SkipAll );
	m_button_Abort = new QPushButton( "Abort", this );
	bSizer2->addWidget( m_button_Abort );
	bSizer1->addLayout( bSizer2 );
	bSizer1->setAlignment( bSizer2, Qt::AlignHCenter );

	// Connect Events
	QObject::connect(m_button_Locate, &QPushButton::clicked, [this]() { buttonLocateClick(); });
	QObject::connect(m_button_Retry, &QPushButton::clicked, [this]() { buttonRetryClick(); });
	QObject::connect(m_button_Skip, &QPushButton::clicked, [this]() { buttonSkipClick(); });
	QObject::connect(m_button_SkipAll, &QPushButton::clicked, [this]() { buttonSkipAllClick(); });
	QObject::connect(m_button_Abort, &QPushButton::clicked, [this]() { buttonAbortClick(); });
}

qtResolvePathDialogBase::~qtResolvePathDialogBase()
{
}

#endif
