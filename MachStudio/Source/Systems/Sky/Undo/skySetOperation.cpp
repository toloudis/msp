/*****************************************************************************
**	skySetOperation.cpp
**
**	 Represents a change of for settings operation, which can be undone
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "skySetOperation.hpp"

#include "skyDataMgr.hpp"

#include "daySkyMgr.hpp"


//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
skySetOperation::skySetOperation(const daySkyLayerList &i_OldData)
: m_DataBackup(i_OldData)
{

}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string  skySetOperation::GetDisplayName()
{
	return "Sky Settings";
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float  skySetOperation::GetMemoryUsage()
{
	int size;
	size = sizeof(daySkyLayerData);
	return ( size / 1000.0f ); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void  skySetOperation::Undo()
{
	daySkyLayerList temp = daySkyMgr::GetListData();
	daySkyMgr::SetListData(m_DataBackup);
	m_DataBackup = temp; // switch backup from undo to redo
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void  skySetOperation::Redo()
{
	daySkyLayerList temp = daySkyMgr::GetListData();
	daySkyMgr::SetListData(m_DataBackup);
	m_DataBackup = temp; // switch backup from rendo to undo
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void  skySetOperation::Commit()
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
void  skySetOperation::Destroy()
{
	// nothing needed
}

