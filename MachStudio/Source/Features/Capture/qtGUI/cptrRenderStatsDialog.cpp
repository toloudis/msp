/*****************************************************************************
**	cptrRenderStatsDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/qtGUI/cptrRenderStatsDialog.hpp"

#ifdef USE_QT

#include <QPlainTextEdit>
#include <QVBoxLayout>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cptrRenderStatsDialog::cptrRenderStatsDialog( QWidget* parent )
:	QDialog(parent)
{
	this->setWindowTitle( "Render Log" );

	QVBoxLayout* sbSizer1 = new QVBoxLayout( this );
	m_richText_stats = new QPlainTextEdit( this );
	m_richText_stats->setReadOnly( true );
	m_richText_stats->setUndoRedoEnabled( false );
	m_richText_stats->setObjectName( "RenderLog" );
	sbSizer1->addWidget( m_richText_stats, 1 );

	this->resize( 412, 477 );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderStatsDialog::append_text( const itString& i_String )
{
	QTextCursor cursor(m_richText_stats->document());
	cursor.movePosition(QTextCursor::End);
	cursor.insertText(QString::fromWCharArray(i_String.GetString()));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderStatsDialog::UpdateStatString( itString& i_pString )
{
	append_text( i_pString );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderStatsDialog::AddStatString( itString& i_pString )
{
	append_text( i_pString );
	append_text( itString(L"\n") );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cptrRenderStatsDialog::Clear()
{
	m_richText_stats->clear();
}

#endif
