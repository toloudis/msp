/*****************************************************************************
**	dynDeleteOperation.hpp
**
**		Classes that will be use to create new undo/redo for delete operations
**	
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef DYN_DELETEOPERATION_HPP
#error dynDeleteOperation.hpp multiply included
#endif
#define DYN_DELETEOPERATION_HPP


#ifndef UNDO_UNDOOPERATION_HPP
#include "Core/undo/undoUndoOperation.hpp"
#endif

#ifndef DYN_CONTROLDATA_HPP
#include "Support/dyn/dynControlData.hpp"
#endif

//class forwards
class dynScriptObject;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class dynDeleteOperation : public undoUndoOperation
{
public:
	//--------------------------------------------------------------------
	// constructor takes old to be restored if undone
	//--------------------------------------------------------------------
	dynDeleteOperation( dynScriptObject* i_pObject,
						std::string i_Name,
						const char* i_DisplayName);

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

	//--------------------------------------------------------------------
	//  Commit is called on an operation when it is no longer
	// possible for the user to undo this operation.  The
	// destructor will soon be called.
	//--------------------------------------------------------------------
	virtual void Commit();

	//--------------------------------------------------------------------
	//  Destroy is called on an operation when it has been undone
	// and it can no longer be redone. This may happen after the
	// history gets long enough or a new operation is made when
	// its current state is "undone". The destructor will soon be
	// called.
	//--------------------------------------------------------------------
	virtual void Destroy();

private:
	int m_Index;
	std::string m_Name;
	dynScriptObject* m_pObject;
	dynControlData m_Data;
	std::string m_DisplayName;
};