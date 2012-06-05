/*****************************************************************************
**	dbgConsoleDialog.hpp
**
**		implementation of the debug console dialog
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef DBG_CONSOLEDIALOG_HPP
#error dbgConsoleDialog.hpp multiply included
#endif
#define DBG_CONSOLEDIALOG_HPP

//	needed before App so USE_WXWIDGETS is set
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//	base dialog
#ifdef USE_WXWIDGETS
#include "Features/Debug/wxGUI/dbgConsoleDialogBase.h"
#endif

#include <string>


#ifdef USE_WXWIDGETS

//============================================================================
// Class RenderStatsDialog
//============================================================================
class dbgConsoleDialog : public dbgConsoleDialogBase
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
		dbgConsoleDialog( wxWindow* parent, const std::string& i_Title = "Debug" );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~dbgConsoleDialog();

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void dbgConsoleDialog_OnActivate( wxActivateEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//~cptrRenderStatsDialog();
};

#endif