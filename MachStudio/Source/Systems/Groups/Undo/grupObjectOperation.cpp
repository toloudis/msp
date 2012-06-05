/*****************************************************************************
**	grupObjectOperation.hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Systems/Groups/Undo/grupObjectOperation.hpp"
#include "Systems/Groups/Undo/grupActualOperations.hpp"
#include "Systems/Groups/GUI/grupDialogUtil.hpp"

#include "Support/grps/grpsGroupMgr.hpp"


//--------------------------------------------------------------------
// constructor 
//--------------------------------------------------------------------
grupObjectOperation::grupObjectOperation( const char* i_DisplayName,
										  const nameString& i_SetName, 
										  const nameString& i_ComponentName,
										  bool i_bAdded)
:	m_DisplayName(i_DisplayName), 
	m_SetName(i_SetName), 
	m_ComponentName(i_ComponentName), 
	m_bAdded( i_bAdded )
{
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string grupObjectOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float grupObjectOperation::GetMemoryUsage()
{
	return (1); // estimate in KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void grupObjectOperation::Undo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void grupObjectOperation::Redo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Do the actual undo or redo based on our stored data and state
//--------------------------------------------------------------------
void grupObjectOperation::toggle_state()
{
	if (m_bAdded)
		grupActualOperations::RemoveObjectFromGroup(m_SetName, m_ComponentName);
	else
		grupActualOperations::AddObjectToGroup(m_SetName, m_ComponentName);

	// toggle state
	m_bAdded = !m_bAdded;

	// update dialog
	grupDialogUtil::UpdateObjectsPage();
}
