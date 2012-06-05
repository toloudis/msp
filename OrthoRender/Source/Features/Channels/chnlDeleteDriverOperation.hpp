/*****************************************************************************
**	chnlDeleteDriverOperation.hpp
**
**	 Undo operation for deleting drivers
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_DELETEDRIVEROPERATION_HPP
#error chnlDeleteDriverOperation.hpp multiply included
#endif
#define CHNL_DELETEDRIVEROPERATION_HPP

#ifndef UNDO_UNDOOPERATION_HPP
#include "Core/undo/undoUndoOperation.hpp"
#endif

class tmlnDriver;
class tmlnDriverInfo;
class tmlnScriptObject;

class chnlDeleteDriverOperation : public undoUndoOperation
{
public:

	//--------------------------------------------------------------------
	// constructor takes old to be restored if undone
	// takes ownership of the DriverInfo, but not the script object.
	//--------------------------------------------------------------------
	chnlDeleteDriverOperation(tmlnScriptObject* i_pObject,
							  tmlnDriverInfo* i_pInfo);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~chnlDeleteDriverOperation();

	//--------------------------------------------------------------------
	//  Get Name for the operation
	//--------------------------------------------------------------------
	virtual std::string  GetDisplayName();

	//--------------------------------------------------------------------
	//  Get memory usage for this operation (in KB). This can be
	// accurate or approximate.
	//--------------------------------------------------------------------
	virtual float  GetMemoryUsage();

	//--------------------------------------------------------------------
	//  Undo is called on an operation when the user chooses
	// Edit->Undo from the menu.
	//--------------------------------------------------------------------
	virtual void  Undo();

	//--------------------------------------------------------------------
	// Redo is called on an operation when the user chooses
	// Edit->Redo from the menu and this operation is the next in
	// line to be redone.
	//--------------------------------------------------------------------
	virtual void  Redo();

	//--------------------------------------------------------------------
	//  Commit is called on an operation when it is no longer
	// possible for the user to undo this operation.  The
	// destructor will soon be called.
	//--------------------------------------------------------------------
	virtual void  Commit();

	//--------------------------------------------------------------------
	//  Destroy is called on an operation when it has been undone
	// and it can no longer be redone. This may happen after the
	// history gets long enough or a new operation is made when
	// its current state is "undone". The destructor will soon be
	// called.
	//--------------------------------------------------------------------
	virtual void  Destroy();


private:
	tmlnScriptObject* m_pObject;
	tmlnDriverInfo* m_pInfo;
	tmlnDriver* m_pDriver;

};
