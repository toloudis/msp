/*****************************************************************************
**	dbgConsoleDialog.hpp
**
**		implementation of the debug console dialog
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef DBG_CONSOLEDIALOG_HPP
#error dbgConsoleDialog.hpp multiply included
#endif
#define DBG_CONSOLEDIALOG_HPP

//	needed before App so USE_QT is set
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <string>

class dbgConsoleBuf;
class dbgStringStream;
class QPlainTextEdit;

#ifdef USE_QT

//============================================================================
// Class dbgConsoleDialog - docked pane showing the debug messages
//============================================================================
class dbgConsoleDialog : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static dbgConsoleDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void UpdateConsoleString( std::string& i_pString );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		dbgConsoleDialog( QWidget* parent,
						  const char* i_Title = "Debug",
						  const char* i_Caption = "Debug Console" );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~dbgConsoleDialog();

	private:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void AddDebugConsoleStream();

		QPlainTextEdit* m_richText_Console;
		dbgConsoleBuf* m_Buf;
		dbgStringStream* m_Dbgs;
};

#endif
