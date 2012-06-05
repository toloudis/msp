/*****************************************************************************
**	lsetNodeOperation.hpp
**
**	 Undoable operation for changing membership in sets.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Undo/lsetNodeOperation.hpp"
#include "Systems/LightSets/Undo/lsetActualOperations.hpp"
#include "Systems/LightSets/GUI/lsetDialogUtil.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"


//--------------------------------------------------------------------
// constructor 
//--------------------------------------------------------------------
lsetNodeOperation::lsetNodeOperation( const char* i_DisplayName,
									  const nameString& i_SetName, 
									  const nameString& i_ComponentName,
									  int i_NodeIndex,
									  bool i_bAdded)
:	m_DisplayName(i_DisplayName), 
	m_SetName(i_SetName), 
	m_ComponentName(i_ComponentName), 
	m_NodeIndex(i_NodeIndex),
	m_bAdded( i_bAdded )
{
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string lsetNodeOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float lsetNodeOperation::GetMemoryUsage()
{
	return (1); // estimate in KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void lsetNodeOperation::Undo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void lsetNodeOperation::Redo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Do the actual undo or redo based on our stored data and state
//--------------------------------------------------------------------
void lsetNodeOperation::toggle_state()
{
	if (m_bAdded)
		lsetActualOperations::RemoveNodeFromLightSet(m_SetName, m_ComponentName, m_NodeIndex);
	else
		lsetActualOperations::AddNodeToLightSet(m_SetName, m_ComponentName, m_NodeIndex);

	// toggle state
	m_bAdded = !m_bAdded;

	// update dialog checkboxes
	lsetDialogUtil::UpdateObjectsPage();
}
