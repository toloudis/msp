/*****************************************************************************
**	trfnObjectOperation.hpp
**
**	 Undoable operation for changing membership in sets.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef TRFN_LIGHTOPERATION_HPP
#error trfnObjectOperation.hpp multiply included
#endif
#define TRFN_LIGHTOPERATION_HPP

#ifndef UNDO_UNDOOPERATION_HPP
#include "Core/undo/undoUndoOperation.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif 


//============================================================================
//============================================================================
class trfnObjectOperation : public undoUndoOperation
{
public:
	//--------------------------------------------------------------------
	// constructor 
	//--------------------------------------------------------------------
	trfnObjectOperation( const char* i_DisplayName, 
					  const nameString& i_ComponentName );

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
	nameString m_TransformName; 
	nameString m_ComponentName;
	bool m_bAdded;
};

//============================================================================
// Convenience classes,set state variables automatically
//============================================================================
class trfnAddObjectOperation : public trfnObjectOperation
{
public:
	trfnAddObjectOperation( const nameString& i_ComponentName)
	 : trfnObjectOperation( "Add Object To Transform", i_ComponentName) {}
};
class trfnRemoveObjectOperation : public trfnObjectOperation
{
public:
	trfnRemoveObjectOperation( const nameString& i_ComponentName)
	 : trfnObjectOperation( "Remove Object From Transform", i_ComponentName) {}
};
