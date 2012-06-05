/*****************************************************************************
**	envtObjectOperation.hpp
**
**	 Undoable operation for changing membership in sets.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Undo/envtObjectOperation.hpp"
#include "Systems/Environments/Undo/envtActualOperations.hpp"
#include "Systems/Environments/GUI/envtDialogUtil.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"


//--------------------------------------------------------------------
// constructor 
//--------------------------------------------------------------------
envtObjectOperation::envtObjectOperation( const char* i_DisplayName,
										  const nameString& i_ComponentName )
:	m_DisplayName(i_DisplayName), 
	m_ComponentName(i_ComponentName)
{
	// Get the light set that currently contains the light in order
	// to restore it later.
	m_bAdded = evmtEnvironmentMgr::GetEnvironmentNameFromObject(i_ComponentName, m_EnvironmentName);
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string envtObjectOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float envtObjectOperation::GetMemoryUsage()
{
	return (1); // estimate in KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void envtObjectOperation::Undo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void envtObjectOperation::Redo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Do the actual undo or redo based on our stored data and state
//--------------------------------------------------------------------
void envtObjectOperation::toggle_state()
{
	// Get light set the light is currently in, this becomes the new backup state
	nameString prev_set;
	bool bAdded = evmtEnvironmentMgr::GetEnvironmentNameFromObject(m_ComponentName, prev_set);

	// Either move the light into its old light set, or remove it from all light sets
	if (m_bAdded)
		envtActualOperations::AddObjectToEnvironment(m_EnvironmentName, m_ComponentName);
	else
		envtActualOperations::RemoveObjectFromEnvironment(prev_set, m_ComponentName);

	// backup old state
	m_bAdded = bAdded;
	m_EnvironmentName = prev_set;

	// update dialog checkboxes
	envtDialogUtil::UpdateObjectsPage();

}
