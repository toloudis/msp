/*****************************************************************************
**	pythPythonDialog.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/qtGUI/pythPythonDialog.hpp"

#include "Support/pyth/pythUtil.hpp"

#include "Core/Env/envString.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"

#ifdef USE_QT

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QVBoxLayout>

//============================================================================
//============================================================================
namespace
{
	// Log text that arrived before the dialog was created
	itString l_LogString;

	QString to_qstring(const itString& i_String)
	{
		return QString::fromWCharArray(i_String.GetString());
	}
}	// end of namespace

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
pythPythonDialog* pythPythonDialog::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
pythPythonDialog::pythPythonDialog( QWidget* parent,
									const char* i_Title )
:	QWidget( parent )
{
	QVBoxLayout* bSizer1 = new QVBoxLayout( this );

	m_textCtrl_PythLog = new QPlainTextEdit( this );
	m_textCtrl_PythLog->setReadOnly( true );
	bSizer1->addWidget( m_textCtrl_PythLog, 1 );

	bSizer1->addWidget( new QLabel( "Python Command:", this ) );

	QHBoxLayout* bSizer2 = new QHBoxLayout();
	m_textCtrl_PythCommand = new QPlainTextEdit( this );
	m_textCtrl_PythCommand->setToolTip( "Ctrl+Enter executes the command" );
	m_textCtrl_PythCommand->installEventFilter( this );
	bSizer2->addWidget( m_textCtrl_PythCommand, 1 );

	QVBoxLayout* bSizer3 = new QVBoxLayout();
	QPushButton* pExecute = new QPushButton( "Execute", this );
	bSizer3->addWidget( pExecute );
	QPushButton* pClear = new QPushButton( "Clear", this );
	bSizer3->addWidget( pClear );
	bSizer3->addStretch( 1 );
	bSizer2->addLayout( bSizer3 );
	bSizer1->addLayout( bSizer2 );

	QObject::connect( pExecute, &QPushButton::clicked, [this]() { ExecuteCommand(); } );
	QObject::connect( pClear, &QPushButton::clicked, [this]()
	{
		m_textCtrl_PythCommand->clear();
		m_textCtrl_PythLog->clear();
	} );

	// Show the log text that arrived before we were created
	if (l_LogString.GetLength() > 0)
	{
		this->AddToLogTextbox(l_LogString);
		l_LogString = itString();
	}

	// Add this panel to the pane manager
	tqtPaneMgr::AddPane(this, tqtPaneInfo().Name(i_Title).Caption(i_Title).Show().Layer(1).Left().BestSize(500, 300));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
pythPythonDialog::~pythPythonDialog()
{
	if (pythPythonDialog::Instance == this)
		pythPythonDialog::Instance = NULL;
}

//--------------------------------------------------------------------
// Add string to log textbox
//--------------------------------------------------------------------
void pythPythonDialog::AddToLogTextbox(const itString& i_String)
{
	QTextCursor cursor(m_textCtrl_PythLog->document());
	cursor.movePosition(QTextCursor::End);
	cursor.insertText(to_qstring(i_String));
	m_textCtrl_PythLog->verticalScrollBar()->setValue(m_textCtrl_PythLog->verticalScrollBar()->maximum());
}

//--------------------------------------------------------------------
// Add string to log. If dialog is not created yet, store the
//	string for later display.
//--------------------------------------------------------------------
//static
void pythPythonDialog::AddToLog(const itString& i_String)
{
	if (pythPythonDialog::Instance != NULL)
	{
		pythPythonDialog::Instance->AddToLogTextbox(i_String);
	}
	else
	{
		l_LogString += i_String;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void pythPythonDialog::ExecuteCommand()
{
	QString command = m_textCtrl_PythCommand->toPlainText().trimmed();

	QString log_string = ">> ";
	log_string += QString(command).replace("\n", "\n> ");
	log_string += "\n";
	this->AddToLogTextbox(itString(log_string.toStdWString().c_str()));

	if (pythUtil::ExecuteCommand( tqtStdString(command) ))
	{
		// If successful, clear text
		m_textCtrl_PythCommand->clear();
	}
}

//--------------------------------------------------------------------
// Handle CTRL-ENTER in the command box as "execute script"
//--------------------------------------------------------------------
bool pythPythonDialog::eventFilter(QObject* i_pWatched, QEvent* i_pEvent)
{
	if (i_pWatched == m_textCtrl_PythCommand && i_pEvent->type() == QEvent::KeyPress)
	{
		QKeyEvent* pKey = static_cast<QKeyEvent*>(i_pEvent);
		if ((pKey->modifiers() & Qt::ControlModifier) &&
			(pKey->key() == Qt::Key_Return || pKey->key() == Qt::Key_Enter))
		{
			this->ExecuteCommand();
			return true;
		}
	}
	return QWidget::eventFilter(i_pWatched, i_pEvent);
}

#endif // USE_QT
