/*****************************************************************************
**	skySetOperation.hpp
**
**	 Represents a change of for settings operation, which can be undone
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef SKY_SETOPERATION_HPP
#error skySetOperation.hpp multiply included
#endif
#define SKY_SETOPERATION_HPP


#ifndef DAY_SKYDATA_HPP
#include "daySkyData.hpp"
#endif
#ifndef UNDO_UNDOOPERATION_HPP
#include "undoUndoOperation.hpp"
#endif


class skySetOperation : public undoUndoOperation
{
public:

	//--------------------------------------------------------------------
	// constructor takes old to be restored if undone
	//--------------------------------------------------------------------
	skySetOperation(const daySkyLayerList &i_OldData);

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
	daySkyLayerList m_DataBackup;

};
