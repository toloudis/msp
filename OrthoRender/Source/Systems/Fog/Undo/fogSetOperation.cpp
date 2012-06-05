/*****************************************************************************
**	fogSetOperation.cpp
**
**	 Represents a change of for settings operation, which can be undone
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Fog/Undo/fogSetOperation.hpp"

#include "Systems/Fog/Data/fogDataMgr.hpp"


//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
fogSetOperation::fogSetOperation(const fogFogData &i_OldData)
: m_DataBackup(i_OldData)
{

}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string  fogSetOperation::GetDisplayName()
{
	return "Fog Settings";
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float  fogSetOperation::GetMemoryUsage()
{
	return (sizeof(fogFogData) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void  fogSetOperation::Undo()
{
	fogFogData temp = fogDataMgr::GetData();
	fogDataMgr::SetData(m_DataBackup);
	m_DataBackup = temp; // switch backup from undo to redo
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void  fogSetOperation::Redo()
{
	fogFogData temp = fogDataMgr::GetData();
	fogDataMgr::SetData(m_DataBackup);
	m_DataBackup = temp; // switch backup from rendo to undo
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void  fogSetOperation::Commit()
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
void  fogSetOperation::Destroy()
{
	// nothing needed
}

