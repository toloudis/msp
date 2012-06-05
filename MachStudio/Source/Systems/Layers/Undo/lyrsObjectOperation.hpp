/*****************************************************************************
**	lyrsObjectOperation.hpp
**
**	 Undoable operation for changing membership in sets.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef LYRS_LIGHTOPERATION_HPP
#error lyrsObjectOperation.hpp multiply included
#endif
#define LYRS_LIGHTOPERATION_HPP

#ifndef UNDO_UNDOOPERATION_HPP
#include "Core/undo/undoUndoOperation.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif 


//============================================================================
//============================================================================
class lyrsObjectOperation : public undoUndoOperation
{
public:
	//--------------------------------------------------------------------
	// constructor 
	//--------------------------------------------------------------------
	lyrsObjectOperation( const char* i_DisplayName, 
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
	nameString m_LayerName; 
	nameString m_ComponentName;
	bool m_bAdded;
};

//============================================================================
// Convenience classes,set state variables automatically
//============================================================================
class lyrsAddObjectOperation : public lyrsObjectOperation
{
public:
	lyrsAddObjectOperation( const nameString& i_ComponentName)
	 : lyrsObjectOperation( "Add Object To Layer", i_ComponentName) {}
};
class lyrsRemoveObjectOperation : public lyrsObjectOperation
{
public:
	lyrsRemoveObjectOperation( const nameString& i_ComponentName)
	 : lyrsObjectOperation( "Remove Object From Layer", i_ComponentName) {}
};
