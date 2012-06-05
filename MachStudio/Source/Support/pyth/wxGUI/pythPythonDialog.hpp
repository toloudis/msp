/*****************************************************************************
**	pythPythonDialog.hpp
**
**	Python script dialog for entering commands, written in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PYTH_PYTHONDIALOG_HPP
#error pythPythonDialog.hpp multiply included
#endif
#define PYTH_PYTHONDIALOG_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "Support/pyth/wxGUI/pythPythonDialogBase.h"


//----------------------------------------------------------------------------
// Class pythPythonDialog
//----------------------------------------------------------------------------
class pythPythonDialog : public pythPythonDialogBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static pythPythonDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pythPythonDialog( wxWindow* parent, 
						  const wxString& i_Title = L"Python" );

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


	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ExecuteCommand();

		//--------------------------------------------------------------------
		// event handlers
		//--------------------------------------------------------------------
		virtual void textCtrl_PythCommand_KeyUp( wxKeyEvent& i_Event );
		virtual void button_ExecutePyth_Click( wxCommandEvent& i_Event );
		virtual void button_ClearPyth_Click( wxCommandEvent& i_Event );

};

#endif // USE_WXWIDGETS
