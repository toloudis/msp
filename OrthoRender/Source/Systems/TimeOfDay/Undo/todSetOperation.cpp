/*****************************************************************************
**	todSetOperation.cpp
**
**	 Represents a change of for settings operation, which can be undone
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "todSetOperation.hpp"

#include "todDataMgr.hpp"


//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
todSetOperation::todSetOperation(const todTimeOfDayData &i_OldData)
: m_DataBackup(i_OldData)
{

}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string  todSetOperation::GetDisplayName()
{
	return "TimeOfDay Settings";
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float  todSetOperation::GetMemoryUsage()
{
	return (sizeof(todTimeOfDayData) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void  todSetOperation::Undo()
{
	todTimeOfDayData temp = todDataMgr::GetData();
	todDataMgr::SetData(m_DataBackup);
	m_DataBackup = temp; // switch backup from undo to redo
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void  todSetOperation::Redo()
{
	todTimeOfDayData temp = todDataMgr::GetData();
	todDataMgr::SetData(m_DataBackup);
	m_DataBackup = temp; // switch backup from rendo to undo
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void  todSetOperation::Commit()
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
void  todSetOperation::Destroy()
{
	// nothing needed
}

