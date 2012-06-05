/*****************************************************************************
**	lsetLightOperation.hpp
**
**	 Undoable operation for changing membership in sets.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Undo/lsetLightOperation.hpp"
#include "Systems/LightSets/Undo/lsetActualOperations.hpp"
#include "Systems/LightSets/GUI/lsetDialogUtil.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"


//--------------------------------------------------------------------
// constructor 
//--------------------------------------------------------------------
lsetLightOperation::lsetLightOperation( const char* i_DisplayName,
								   const nameString& i_ComponentName,
								   bool i_bAdded )
:	m_DisplayName(i_DisplayName), 
	m_ComponentName(i_ComponentName), 
	m_bAdded( i_bAdded )
{
	// Get the light set that currently contains the light in order
	// to restore it later.
	m_bAdded = ltstLightSetMgr::GetSetNameFromLight(i_ComponentName, m_SetName);
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string lsetLightOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float lsetLightOperation::GetMemoryUsage()
{
	return (1); // estimate in KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void lsetLightOperation::Undo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void lsetLightOperation::Redo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Do the actual undo or redo based on our stored data and state
//--------------------------------------------------------------------
void lsetLightOperation::toggle_state()
{
	// Get light set the light is currently in, this becomes the new backup state
	nameString prev_set;
	bool bAdded = ltstLightSetMgr::GetSetNameFromLight(m_ComponentName, prev_set);

	// Either move the light into its old light set, or remove it from all light sets
	if (m_bAdded)
		lsetActualOperations::AddLightToSet(m_SetName, m_ComponentName);
	else
		lsetActualOperations::RemoveLightFromSet(prev_set, m_ComponentName);

	// backup old state
	m_bAdded = bAdded;
	m_SetName = prev_set;

	// update dialog checkboxes
	lsetDialogUtil::UpdateLightsPage();

}
