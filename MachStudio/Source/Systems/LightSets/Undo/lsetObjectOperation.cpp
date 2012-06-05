/*****************************************************************************
**	lsetObjectOperation.hpp
**
**	 Undoable operation for changing membership in sets.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Undo/lsetObjectOperation.hpp"
#include "Systems/LightSets/Undo/lsetActualOperations.hpp"
#include "Systems/LightSets/GUI/lsetDialogUtil.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"


//--------------------------------------------------------------------
// constructor 
//--------------------------------------------------------------------
lsetObjectOperation::lsetObjectOperation( const char* i_DisplayName,
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
std::string lsetObjectOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float lsetObjectOperation::GetMemoryUsage()
{
	return (1); // estimate in KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void lsetObjectOperation::Undo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void lsetObjectOperation::Redo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Do the actual undo or redo based on our stored data and state
//--------------------------------------------------------------------
void lsetObjectOperation::toggle_state()
{
	if (m_bAdded)
		lsetActualOperations::RemoveObjectFromLightSet(m_SetName, m_ComponentName);
	else
		lsetActualOperations::AddObjectToLightSet(m_SetName, m_ComponentName);

	// toggle state
	m_bAdded = !m_bAdded;

	// update dialog
	lsetDialogUtil::UpdateObjectsPage();
}
