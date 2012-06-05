/*****************************************************************************
**	UndoHistoryDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/UndoHistory/UndoHistoryDialogUtil.hpp"

#include "Features/UndoHistory/wxGUI/UndoHistoryDialog.hpp"
//#include "Features/UndoHistory/UndoHistoryUtil.hpp"
//#include "Core/undo/undoUndoMgr.hpp"

#include "Tool/gui/guiMenuMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

// Includes from old managed file
#ifndef UNDOHISTORYUTIL_HPP
#include "Features/UndoHistory/UndoHistoryUtil.hpp"
#endif
#ifndef UNDO_UNDOMGR_HPP
#include "Core/undo/undoUndoMgr.hpp"
#endif

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
		//UndoHistoryDialogUtil::UpdateForm();
		UndoHistoryUtil::UpdateMenuItems();
	}

	//--------------------------------------------------------------------
	//	StackChanged - undo stack changed
	//--------------------------------------------------------------------
	//virtual 
	void UndoHistoryInterest::StackChanged()
	{
		//UndoHistoryDialogUtil::UpdateForm();
		UndoHistoryUtil::UpdateMenuItems();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{

#ifdef USE_WXWIDGETS
		UndoHistoryDialog* pUndoDialog = new UndoHistoryDialog( twxSystem::g_pMainForm );
		pUndoDialog->Show();
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

#ifdef USE_WXWIDGETS
		//if (UndoHistoryDialog::FormInstance != NULL)
		//	UndoHistoryDialog::FormInstance->UpdateList();
#endif
	}

}	// end of namespace

