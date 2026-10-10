/*****************************************************************************
**	pythPythonDialog.hpp
**
**	Python script dialog for entering commands, written in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_PYTHONDIALOG_HPP
#error pythPythonDialog.hpp multiply included
#endif
#define PYTH_PYTHONDIALOG_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

class QPlainTextEdit;

#ifdef USE_QT

//----------------------------------------------------------------------------
// Class pythPythonDialog - docked pane for running python commands
//----------------------------------------------------------------------------
class pythPythonDialog : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static pythPythonDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pythPythonDialog( QWidget* parent,
						  const char* i_Title = "Python" );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~pythPythonDialog();

		//--------------------------------------------------------------------
		// Add string to log textbox
		//--------------------------------------------------------------------
		void AddToLogTextbox(const itString& i_String);

		//--------------------------------------------------------------------
		// Add string to log. If dialog is not created yet, store the
		//	string for later display.
		//--------------------------------------------------------------------
		static void AddToLog(const itString& i_String);

	protected:
		bool eventFilter(QObject* i_pWatched, QEvent* i_pEvent) override;

	private:
		void ExecuteCommand();

		QPlainTextEdit* m_textCtrl_PythLog;
		QPlainTextEdit* m_textCtrl_PythCommand;
};

#endif // USE_QT
