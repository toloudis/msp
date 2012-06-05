/*****************************************************************************
**	trfnObjectOperation.hpp
**
**	 Undoable operation for changing membership in sets.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/Undo/trfnObjectOperation.hpp"
#include "Systems/Transforms/Undo/trfnActualOperations.hpp"

#include "Support/xfrm/xfrmTransformMgr.hpp"

//--------------------------------------------------------------------
// constructor 
//--------------------------------------------------------------------
trfnObjectOperation::trfnObjectOperation( const char* i_DisplayName,
								   const nameString& i_ComponentName )
:	m_DisplayName(i_DisplayName), 
	m_ComponentName(i_ComponentName)
{
	// Get the light set that currently contains the light in order
	// to restore it later.
	m_bAdded = xfrmTransformMgr::GetParentForNode(i_ComponentName, m_TransformName);
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string trfnObjectOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float trfnObjectOperation::GetMemoryUsage()
{
	return (1); // estimate in KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void trfnObjectOperation::Undo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void trfnObjectOperation::Redo()
{
	this->toggle_state();
}

//--------------------------------------------------------------------
// Do the actual undo or redo based on our stored data and state
//--------------------------------------------------------------------
void trfnObjectOperation::toggle_state()
{
	// Get light set the light is currently in, this becomes the new backup state
	nameString prev_set;
	bool bAdded = xfrmTransformMgr::GetParentForNode(m_ComponentName, prev_set);

	// Either move the light into its old light set, or remove it from all light sets
	if (m_bAdded)
		trfnActualOperations::AddNodeToTransform(m_TransformName, m_ComponentName);
	else
		trfnActualOperations::RemoveNodeFromParent(m_ComponentName);

	// backup old state
	m_bAdded = bAdded;
	m_TransformName = prev_set;

}
