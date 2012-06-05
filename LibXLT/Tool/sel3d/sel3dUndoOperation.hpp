/*****************************************************************************
**	sel3dUndoOperation.hpp
**
**		store selection list for undo/redo
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SEL3D_UNDOOPERATION_HPP
#error sel3dUndoOperation.hpp multiply included
#endif
#define SEL3D_UNDOOPERATION_HPP

#ifndef UNDO_UNDOOPERATION_HPP
#include "Core/undo/undoUndoOperation.hpp"
#endif
#ifndef REL_OBJECTREFERENCE_HPP
#include "Core/rel/relObjectReference.hpp"
#endif 

#include <list>


//============================================================================
//============================================================================
class sel3dObject;


//============================================================================
//============================================================================
class sel3dUndoOperation : public undoUndoOperation
{
public:

	//--------------------------------------------------------------------
	// constructor takes old to be restored if undone
	//--------------------------------------------------------------------
	sel3dUndoOperation(std::list<sel3dObject*>& i_SelList);

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
	//std::list<sel3dObject*>	m_Backup;
	std::list< shared_ptr<relObjectReference> > m_Backup;

};
