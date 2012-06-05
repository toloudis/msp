/*****************************************************************************
**	setsDeleteOperation.cpp
**
**	 Represents the add set item  operation, which can be undone
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/Undo/setsDeleteOperation.hpp"

#include "Systems/Sets/Data/setsDataMgr.hpp"


namespace
{
	//============================================================================
	//============================================================================
	const char *c_DeleteOperationDisplayName = "Delete Set Item";
}

//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
setsDeleteOperation::setsDeleteOperation(const setsScriptData &i_OldData)
:	m_ItemBackup(i_OldData)
{
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string  setsDeleteOperation::GetDisplayName()
{
	std::string delete_operation_name(c_DeleteOperationDisplayName);
	std::string item_backup_name = m_ItemBackup.m_BaseData.m_Name.GetString();
	if(item_backup_name != "")
		delete_operation_name += " - ";
	return (delete_operation_name + item_backup_name);
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float  setsDeleteOperation::GetMemoryUsage()
{
	// really needs size of filename, but there isn't much
	// memory used no matter what
	return (sizeof(m_ItemBackup) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void  setsDeleteOperation::Undo()
{
	setsDataMgr::AddSetItem( m_ItemBackup );
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void  setsDeleteOperation::Redo()
{
	setsDataMgr::RemoveSetItem( m_ItemBackup );
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void  setsDeleteOperation::Commit()
{
	// nothing needed
}

//--------------------------------------------------------------------
//  Destroy is called on an operation when it has been undone
// and it can no longer be redone. This may happen after the
// history gets long enough or a new operation is made when
// its current state is "undone". The destructor will soon be
// called.
//--------------------------------------------------------------------
void  setsDeleteOperation::Destroy()
{
	// nothing needed
}

