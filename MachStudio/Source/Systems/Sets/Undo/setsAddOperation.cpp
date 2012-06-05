/*****************************************************************************
**	setsAddOperation.cpp
**
**	 Represents the add set item  operation, which can be undone
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/Undo/setsAddOperation.hpp"

#include "Systems/Sets/Data/setsDataMgr.hpp"


namespace
{
	//============================================================================
	//============================================================================
	const char *c_AddOperationDisplayName = "Add Set Item";
}

//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
setsAddOperation::setsAddOperation(const setsScriptData &i_OldData)
:	m_ItemBackup(i_OldData)
{
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string  setsAddOperation::GetDisplayName()
{
	std::string add_operation_name(c_AddOperationDisplayName);
	std::string item_backup_name = m_ItemBackup.m_BaseData.m_Name.GetString();
	if(item_backup_name != "")
		add_operation_name += " - ";
	return (add_operation_name + item_backup_name);
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float  setsAddOperation::GetMemoryUsage()
{
	// really needs size of filename, but there isn't much
	// memory used no matter what
	return (sizeof(m_ItemBackup) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void  setsAddOperation::Undo()
{
	setsDataMgr::RemoveSetItem( m_ItemBackup );
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void  setsAddOperation::Redo()
{
	setsDataMgr::AddSetItem( m_ItemBackup );
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void  setsAddOperation::Commit()
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
void  setsAddOperation::Destroy()
{
	// nothing needed
}

