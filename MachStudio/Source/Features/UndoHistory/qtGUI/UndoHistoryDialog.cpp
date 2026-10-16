/*****************************************************************************
**	UndoHistoryDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/UndoHistory/qtGUI/UndoHistoryDialog.hpp"

#include "Features/UndoHistory/UndoHistoryData.hpp"
#include "Features/UndoHistory/UndoHistoryUtil.hpp"

#include "Core/Undo/undoUndoMgr.hpp"


#ifdef USE_QT

#include <QEvent>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
UndoHistoryDialog* UndoHistoryDialog::FormInstance = NULL;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
UndoHistoryDialog::UndoHistoryDialog( QWidget* parent )
:	QDialog(parent)
{
	this->setAttribute( Qt::WA_DeleteOnClose );
	this->setWindowTitle( "Action History" );

	QVBoxLayout* bSizer3 = new QVBoxLayout( this );

	m_listBox_UndoList = new QListWidget( this );
	bSizer3->addWidget( m_listBox_UndoList, 1 );

	QHBoxLayout* gSizer5 = new QHBoxLayout();
	m_button_Undo = new QPushButton( "Undo", this );
	m_button_Undo->setAutoDefault( false );
	gSizer5->addWidget( m_button_Undo );
	m_button_Redo = new QPushButton( "Redo", this );
	m_button_Redo->setAutoDefault( false );
	gSizer5->addWidget( m_button_Redo );
	bSizer3->addLayout( gSizer5 );
	bSizer3->setAlignment( gSizer5, Qt::AlignHCenter );

	this->resize( 314, 300 );

	QObject::connect( m_button_Undo, &QPushButton::clicked, []() { undoUndoMgr::Undo(); } );
	QObject::connect( m_button_Redo, &QPushButton::clicked, []() { undoUndoMgr::Redo(); } );

	UndoHistoryDialog::FormInstance = this;

	UpdateList();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
UndoHistoryDialog::~UndoHistoryDialog()
{
	if (UndoHistoryDialog::FormInstance == this)
		UndoHistoryDialog::FormInstance = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void UndoHistoryDialog::UpdateList()
{
	UndoHistoryData data;
	UndoHistoryUtil::BuildDataList( data );

	m_listBox_UndoList->clear();
	for (int i = static_cast<int>(data.m_HistoryList.size()) - 1; i >= 0; --i)
	{
		m_listBox_UndoList->addItem( tqtQString(data.m_HistoryList[i].m_Name) );

		//	if there is a match in index then everything previous in the list can
		//	be undone and everything after can be redone
		if (data.m_NextUndoIndex == i)
		{
			m_listBox_UndoList->addItem( "-----------------------------" );
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual
bool UndoHistoryDialog::event( QEvent* i_pEvent )
{
	if (i_pEvent->type() == QEvent::WindowActivate)
		UpdateList();
	return QDialog::event(i_pEvent);
}

#endif
