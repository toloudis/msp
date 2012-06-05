/*****************************************************************************
**	dynCreateOperation.cpp
**
**		see .hpp
**	
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "Support/dyn/GUI/dynCreateOperation.hpp"

#include "Support/dyn/GUI/dynOperations.hpp"
#include "Support/dyn/dynScriptObject.hpp"

//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
dynCreateOperation::dynCreateOperation( dynScriptObject* i_pObject,
										std::string i_Name,
										const char* i_DisplayName)
:	m_pObject(i_pObject),
	m_Name(i_Name),
	m_DisplayName(i_DisplayName)
{
	m_Index = m_pObject->GetIndexForName(m_Name);
	m_Data = m_pObject->GetControlData(m_Index);
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string dynCreateOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}	

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float dynCreateOperation::GetMemoryUsage()
{
	return (sizeof(dynScriptObject) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void dynCreateOperation::Undo()
{
	const bool bDeleteDrivers = true;
	m_pObject->DeleteControl(m_Name, bDeleteDrivers);
	dynOperations::UpdatePlacedDialog();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void dynCreateOperation::Redo()
{
	m_pObject->AddControl(m_Data);
	dynOperations::UpdatePlacedDialog();
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void dynCreateOperation::Commit()
{

}

//--------------------------------------------------------------------
//  Destroy is called on an operation when it has been undone
// and it can no longer be redone. This may happen after the
// history gets long enough or a new operation is made when
// its current state is "undone". The destructor will soon be
// called.
//--------------------------------------------------------------------
void dynCreateOperation::Destroy()
{

}