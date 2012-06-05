/*****************************************************************************
**	todOperations.cpp
**
**	Utility for operations that are undoable in tod system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "todOperations.hpp"

#include "todDataMgr.hpp"
#include "todSetOperation.hpp"

#include "undoUndoMgr.hpp"

namespace todOperations
{

	namespace
	{
		undoUndoOperation *l_LastOp = NULL;

	}	// end of namespace

	//--------------------------------------------------------------------
	//	TimeOfDay operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp()
	{
		l_LastOp = NULL;
	}

	//--------------------------------------------------------------------
	//  Change auto or fixed
	//--------------------------------------------------------------------
	void  ChangeTimeOfDayAutoOrFixed( bool i_bAutoSet )
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new todSetOperation(todDataMgr::GetData());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		todDataMgr::SetTimeOfDayAutoOrFixed(i_bAutoSet);
	}

	//--------------------------------------------------------------------
	//  Change length of the day
	//--------------------------------------------------------------------
	void  ChangeTimeOfDayLength(float i_fMinutes)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new todSetOperation(todDataMgr::GetData());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		todDataMgr::SetTimeOfDayLength(i_fMinutes);
	}

	//--------------------------------------------------------------------
	//  Change the actual start time of day
	//--------------------------------------------------------------------
	void  ChangeTimeOfDayStartTimeOfDay(float i_fTimeOfDayStart)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new todSetOperation(todDataMgr::GetData());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		todDataMgr::SetTimeOfDayStartTimeOfDay(i_fTimeOfDayStart);
	}

	//--------------------------------------------------------------------
	//  Change enabled auto rotation
	//--------------------------------------------------------------------
	void  ChangeTimeOfDayEnabled(bool i_bEnableAuto)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new todSetOperation(todDataMgr::GetData());
			undoUndoMgr::AddOperation(l_LastOp);
		}
		todDataMgr::SetTimeOfDayEnabled(i_bEnableAuto);
	}

}
