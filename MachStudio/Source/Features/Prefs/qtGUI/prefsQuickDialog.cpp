/*****************************************************************************
**  prefsQuickDialog.cpp
**
**     see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "Features/Prefs/qtGUI/prefsQuickDialog.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Core/dbg/dbgMsg.hpp"


#ifdef USE_QT

#include <QGridLayout>
#include <QMouseEvent>
#include <QPushButton>

//virtual
void prefsQuickDialog::mousePressEvent( QMouseEvent* event )
{
	if (event->button() == Qt::MiddleButton)
		this->reject();
	else
		QDialog::mousePressEvent(event);
}

void prefsQuickDialog::execute_command(const std::string& i_Tag)
{
	//	close this form, then execute
	this->accept();
	cmaCommandMgr::ExecuteCommand(i_Tag, -1);
}

prefsQuickDialog::prefsQuickDialog( QWidget* parent )
: QDialog( parent, Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint )
{
	m_Sizer = new QGridLayout( this );
	m_Sizer->setContentsMargins( 0, 0, 0, 0 );
}

prefsQuickDialog::~prefsQuickDialog()
{
}


void prefsQuickDialog::BuildButtons(const prefsQuickData& i_Data, const int i_ScreenX, const int i_ScreenY)
{
	const int lc_BUTTON_WIDTH = 90;
	const int lc_BUTTON_HEIGHT = 38;
	const int lc_COLUMNS = 4;

	//	build the buttons
	//
	int btncnt = 0;
	for (int j = 0; j < i_Data.m_Commands.GetNumberOfItems(); ++j)
	{
		if (i_Data.m_Commands.GetValueFlag(j) == true)
		{
			const std::string tag = i_Data.m_Commands.GetValueText(j);
			QPushButton* pButton = new QPushButton( tqtQString(tag), this );
			pButton->setObjectName( tqtQString(tag) );
			pButton->setMinimumSize( lc_BUTTON_WIDTH, lc_BUTTON_HEIGHT );
			QObject::connect(pButton, &QPushButton::clicked, [this, tag]() { execute_command(tag); });

			m_Sizer->addWidget( pButton, btncnt / lc_COLUMNS, btncnt % lc_COLUMNS );
			++btncnt;
		}
	}

	// Center the buttons on the given screen position
	this->adjustSize();
	this->move( i_ScreenX - this->width() / 2, i_ScreenY - this->height() / 2 );
}

#endif // USE_QT
