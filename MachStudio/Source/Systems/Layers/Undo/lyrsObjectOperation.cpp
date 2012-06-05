/*****************************************************************************
**	lyrsObjectOperation.hpp
**
**	 Undoable operation for changing membership in sets.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Systems/Layers/Undo/lyrsObjectOperation.hpp"
#include "Systems/Layers/Undo/lyrsActualOperations.hpp"
#include "Systems/Layers/GUI/lyrsDialogUtil.hpp"

#include "Support/lyer/lyerLayerMgr.hpp"


//--------------------------------------------------------------------
// constructor 
//--------------------------------------------------------------------
lyrsObjectOperation::lyrsObjectOperation( const char* i_DisplayName,
								   const nameString& i_ComponentName )
:	m_DisplayName(i_DisplayName), 
	m_ComponentName(i_ComponentName)
{
	// Get the light set that currently contains the light in order
	// to restore it later.
	m_bAdded = lyerLayerMgr::GetLayerNameFromObject(i_ComponentName, m_LayerName);
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string lyrsObjectOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float lyrsObjectOperation::GetMemoryUsage()
{
	return (1); // estimate in KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void lyrsObjectOperation::Undo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void lyrsObjectOperation::Redo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Do the actual undo or redo based on our stored data and state
//--------------------------------------------------------------------
void lyrsObjectOperation::toggle_state()
{
	// Get light set the light is currently in, this becomes the new backup state
	nameString prev_set;
	bool bAdded = lyerLayerMgr::GetLayerNameFromObject(m_ComponentName, prev_set);

	// Either move the light into its old light set, or remove it from all light sets
	if (m_bAdded)
		lyrsActualOperations::AddObjectToLayer(m_LayerName, m_ComponentName);
	else
		lyrsActualOperations::RemoveObjectFromLayer(prev_set, m_ComponentName);

	// backup old state
	m_bAdded = bAdded;
	m_LayerName = prev_set;

	// update dialog checkboxes
	lyrsDialogUtil::UpdateObjectsPage();

}
