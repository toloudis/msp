/*****************************************************************************
**	UndoHistoryDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/UndoHistory/UndoHistoryDialogUtil.hpp"

#include "Features/UndoHistory/mGUI/UndoHistoryForm.h"
#include "Features/UndoHistory/wxGUI/UndoHistoryDialog.hpp"
//#include "Features/UndoHistory/UndoHistoryUtil.hpp"
//#include "Core/undo/undoUndoMgr.hpp"

#include "ToolUIManaged/tma/tmaSystem.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


//
namespace UndoHistoryDialogUtil
{
	namespace
	{
		UndoHistoryData l_Data;
		UndoHistoryInterest* l_pInterest = 0;
	}

	//--------------------------------------------------------------------
	//	UndoAdded - undo event added to the stack
	//--------------------------------------------------------------------
	//virtual 
	void UndoHistoryInterest::UndoAdded()
	{
		UndoHistoryDialogUtil::UpdateForm();
	}

	//--------------------------------------------------------------------
	//	StackChanged - undo stack changed
	//--------------------------------------------------------------------
	//virtual 
	void UndoHistoryInterest::StackChanged()
	{
		UndoHistoryDialogUtil::UpdateForm();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
#ifdef _MANAGED
		Features::UndoHistoryForm^ pUndoHistory = gcnew Features::UndoHistoryForm(l_Data);

		// TODO [rjk] - how does mainform handle it if this form gets delete or goes away?
		tmaSystem::g_pMainForm->AddOwnedForm(pUndoHistory);

		pUndoHistory->Show();
#endif
#ifdef USE_WXWIDGETS
//WXGUI
/*
		UndoHistoryDialog* pUndoDialog = new UndoHistoryDialog( twxSystem::g_pMainForm );
		pUndoDialog->Show();
*/
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{
		//pUndoHistory->Hide();
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
		l_pInterest = new UndoHistoryInterest();
		undoUndoMgr::AddInterest(l_pInterest);
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
#ifdef _MANAGED
		Features::UndoHistoryForm::FormInstance = nullptr;
#endif
#ifdef USE_WXWIDGETS
		//UndoHistoryDialog::FormInstance = NULL;
#endif
		undoUndoMgr::RemoveInterest(l_pInterest);
		delete l_pInterest;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateForm()
	{
#ifdef _MANAGED
		// FIX [rjk] - this crashes when deleting object that has a property on the stack --> Features::UndoHistoryUtil::BuildDataList( l_Data );
		if (Features::UndoHistoryForm::FormInstance)
			Features::UndoHistoryForm::FormInstance->UpdateList();
#endif
#ifdef USE_WXWIDGETS
		//if (UndoHistoryDialog::FormInstance != NULL)
		//	UndoHistoryDialog::FormInstance->UpdateList();
#endif
	}

}	// end of namespace

