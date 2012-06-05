/*****************************************************************************
**	UndoHistoryDialog.hpp
**
**		implementation of the UndoHistory dialog
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_UNDOHISTORYDIALOG_HPP
#error UndoHistoryDialog.hpp multiply included
#endif
#define CPTR_UNDOHISTORYDIALOG_HPP


//	needed before App so USE_WXWIDGETS is set
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//	base dialog
#ifdef USE_WXWIDGETS
#include "Features/UndoHistory/wxGUI/UndoHistoryDialogBase.h"
#endif

#include <string>


#ifdef USE_WXWIDGETS

//============================================================================
// Class RenderStatsDialog
//============================================================================
class UndoHistoryDialog : public UndoHistoryDialogBase
{
	public:
		static UndoHistoryDialog* FormInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		UndoHistoryDialog( wxWindow* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~UndoHistoryDialog();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void UpdateList();

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void UndoHistoryDialog_OnActivate( wxActivateEvent& event );
		virtual void button_Undo_OnButtonClick( wxCommandEvent& event );
		virtual void button_Redo_OnButtonClick( wxCommandEvent& event );
};

#endif
