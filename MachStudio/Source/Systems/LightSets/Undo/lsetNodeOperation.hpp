/*****************************************************************************
**	lsetNodeOperation.hpp
**
**	 Undoable operation for changing membership in sets.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef LSET_NODEOPERATION_HPP
#error lsetNodeOperation.hpp multiply included
#endif
#define LSET_NODEOPERATION_HPP

#ifndef UNDO_UNDOOPERATION_HPP
#include "Core/undo/undoUndoOperation.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif 


//============================================================================
//============================================================================
class lsetNodeOperation : public undoUndoOperation
{
public:
	//--------------------------------------------------------------------
	// constructor 
	//--------------------------------------------------------------------
	lsetNodeOperation( const char* i_DisplayName, 
					  const nameString& i_SetName, 
					  const nameString& i_ComponentName,
					  int i_NodeIndex,
					  bool i_bAdded );

	//--------------------------------------------------------------------
	//  Get Name for the operation
	//--------------------------------------------------------------------
	virtual std::string GetDisplayName();

	//--------------------------------------------------------------------
	//  Get memory usage for this operation (in KB). This can be
	// accurate or approximate.
	//--------------------------------------------------------------------
	virtual float GetMemoryUsage();

	//--------------------------------------------------------------------
	//  Undo is called on an operation when the user chooses
	// Edit->Undo from the menu.
	//--------------------------------------------------------------------
	virtual void Undo();

	//--------------------------------------------------------------------
	// Redo is called on an operation when the user chooses
	// Edit->Redo from the menu and this operation is the next in
	// line to be redone.
	//--------------------------------------------------------------------
	virtual void Redo();

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void toggle_state();

	std::string m_DisplayName;
	nameString m_SetName; 
	nameString m_ComponentName;
	int m_NodeIndex;
	bool m_bAdded;
};

//============================================================================
// Convenience classes,set state variables automatically
//============================================================================
class lsetAddNodeOperation : public lsetNodeOperation
{
public:
	lsetAddNodeOperation( const nameString& i_SetName, 
						    const nameString& i_ComponentName,
							int i_NodeIndex)
	 : lsetNodeOperation( "Add Node To Light Set", i_SetName, i_ComponentName, i_NodeIndex, true) {}
};
class lsetRemoveNodeOperation : public lsetNodeOperation
{
public:
	lsetRemoveNodeOperation( const nameString& i_SetName, 
						    const nameString& i_ComponentName,
							int i_NodeIndex)
	 : lsetNodeOperation( "Remove Node From Light Set", i_SetName, i_ComponentName, i_NodeIndex, false) {}
};
