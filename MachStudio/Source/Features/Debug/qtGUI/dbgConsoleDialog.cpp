/*****************************************************************************
**	dbgConsoleDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Debug/qtGUI/dbgConsoleDialog.hpp"

#include "Features/Debug/dbgConsoleDialogUtil.hpp"
#include "Features/Debug/dbgConsoleStream.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"


#ifdef USE_QT

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTextCharFormat>
#include <QVBoxLayout>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	QColor determineColor(std::string& i_pString, bool &o_bErrorMessage)
	{
		o_bErrorMessage = false;
		if(i_pString.find("*WARNING*",0) != std::string::npos)
		{
			//return warning color
			return QColor(130, 0, 0);
		}
		if(i_pString.find("*ERROR*",0) != std::string::npos)
		{
			//return error color
			o_bErrorMessage = true;
			return QColor(255, 0, 0);
		}
		// default text color of the palette, so it reads in dark themes too
		return QColor();
	}
}

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
dbgConsoleDialog* dbgConsoleDialog::Instance = NULL;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void dbgConsoleDialog::UpdateConsoleString( std::string& i_pString )
{
	bool bErrorMsg = false;
	QColor textCol = determineColor(i_pString, bErrorMsg);

	QTextCharFormat format;
	if (textCol.isValid())
		format.setForeground(textCol);

	QTextCursor cursor(m_richText_Console->document());
	cursor.movePosition(QTextCursor::End);
	cursor.insertText(tqtQString(i_pString), format);
	m_richText_Console->verticalScrollBar()->setValue(m_richText_Console->verticalScrollBar()->maximum());

	// Also display error messages in the status bar
	if (bErrorMsg)
	{
		// This method draws the users attention to the debug console
		guiStatusBarMgr::SetErrorMessage("Errors have occurred, view the debug console for details.");
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dbgConsoleDialog::dbgConsoleDialog( QWidget* parent, const char* i_Title,
									const char* i_Caption )
:	QWidget(parent), m_Buf(NULL), m_Dbgs(NULL)
{
	QVBoxLayout* m_boxSizer_Main = new QVBoxLayout( this );
	m_boxSizer_Main->setContentsMargins( 5, 5, 5, 5 );

	m_richText_Console = new QPlainTextEdit( this );
	m_richText_Console->setReadOnly( true );
	m_richText_Console->setUndoRedoEnabled( false );
	m_richText_Console->setLineWrapMode( QPlainTextEdit::NoWrap );
	m_richText_Console->setFrameShape( QFrame::NoFrame );
	m_boxSizer_Main->addWidget( m_richText_Console, 1 );

	// Add this panel to the pane manager
	tqtPaneMgr::AddPane(this, tqtPaneInfo().Name(i_Title).Caption(i_Caption).Show().Layer(1).Left().BestSize(500, 300));
	AddDebugConsoleStream();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dbgConsoleDialog::~dbgConsoleDialog()
{
	dbgMsg::removeStream(std::wstring(L"debugConsole"));
	if (dbgConsoleDialog::Instance == this)
		dbgConsoleDialog::Instance = NULL;

	delete m_Buf;
	delete m_Dbgs;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void dbgConsoleDialog::AddDebugConsoleStream()
{
	m_Buf = new dbgConsoleBuf(&dbgConsoleDialogUtil::UpdateConsoleDialog);
	m_Dbgs = new dbgStringStream(m_Buf);

	dbgMsg::addStream(std::wstring(L"debugConsole"), m_Dbgs);
}

#endif
