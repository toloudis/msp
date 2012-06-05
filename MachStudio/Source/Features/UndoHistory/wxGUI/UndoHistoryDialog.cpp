/*****************************************************************************
**	UndoHistoryDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/UndoHistory/wxGUI/UndoHistoryDialog.hpp"

#include "Features/UndoHistory/UndoHistoryData.hpp"
#include "Features/UndoHistory/UndoHistoryUtil.hpp"

#include "Core/Undo/undoUndoMgr.hpp"


#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
UndoHistoryDialog* UndoHistoryDialog::FormInstance = NULL;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
UndoHistoryDialog::UndoHistoryDialog( wxWindow* parent )
:	UndoHistoryDialogBase(parent)
{
	UndoHistoryDialog::FormInstance = this;

	UpdateList();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
UndoHistoryDialog::~UndoHistoryDialog()
{
	UndoHistoryDialog::FormInstance = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void UndoHistoryDialog::UpdateList()
{
	UndoHistoryData data;
	UndoHistoryUtil::BuildDataList( data );

	m_listBox_UndoList->Clear();
	for (int i = data.m_HistoryList.size() - 1; i >= 0; --i)
	{
		m_listBox_UndoList->Append( wxString(data.m_HistoryList[i].m_Name.c_str(), wxConvUTF8) );

		//	if there is a match in index then everything previous in the list can
		//	be undone and everything after can be redone
		if (data.m_NextUndoIndex == i)
		{
			m_listBox_UndoList->Append( L"-----------------------------" );
		}
	}

	float curr_mem = undoUndoMgr::GetCurrentMemoryUsage();
	float max_mem = undoUndoMgr::GetMaximumMemoryUsage();
	if ( max_mem > 0 )
	{
		//statusBar_history->Text = System::String::Format( "memory {0:F2}/{1:F2} Kb", curr_mem, max_mem );
	}
	else
	{
		//statusBar_history->Text = System::String::Format( "count {0}  memory {1:F2} Kb", data.m_HistoryList.size(), curr_mem );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void UndoHistoryDialog::UndoHistoryDialog_OnActivate( wxActivateEvent& event )
{ 
	UpdateList();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual 
void UndoHistoryDialog::button_Undo_OnButtonClick( wxCommandEvent& event )
{ 
	undoUndoMgr::Undo();
}

//virtual 
void UndoHistoryDialog::button_Redo_OnButtonClick( wxCommandEvent& event )
{ 
	undoUndoMgr::Redo();
}

#endif


