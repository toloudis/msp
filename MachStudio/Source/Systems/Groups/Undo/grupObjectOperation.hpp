/*****************************************************************************
**	grupObjectOperation.hpp
**
**	 Undoable operation for changing membership in sets.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GRUP_OBJECTOPERATION_HPP
#error grupObjectOperation.hpp multiply included
#endif
#define GRUP_OBJECTOPERATION_HPP

#ifndef UNDO_UNDOOPERATION_HPP
#include "Core/undo/undoUndoOperation.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif 


//============================================================================
//============================================================================
class grupObjectOperation : public undoUndoOperation
{
public:
	//--------------------------------------------------------------------
	// constructor 
	//--------------------------------------------------------------------
	grupObjectOperation( const char* i_DisplayName, 
					  const nameString& i_SetName, 
					  const nameString& i_ComponentName,
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
	bool m_bAdded;
};

//============================================================================
// Convenience classes,set state variables automatically
//============================================================================
class grupAddObjectOperation : public grupObjectOperation
{
public:
	grupAddObjectOperation( const nameString& i_SetName, 
						   const nameString& i_ComponentName)
	 : grupObjectOperation( "Add Object To Group", i_SetName, i_ComponentName, true) {}
};
class grupRemoveObjectOperation : public grupObjectOperation
{
public:
	grupRemoveObjectOperation( const nameString& i_SetName, 
						   const nameString& i_ComponentName)
	 : grupObjectOperation( "Remove Object From Group", i_SetName, i_ComponentName, false) {}
};
