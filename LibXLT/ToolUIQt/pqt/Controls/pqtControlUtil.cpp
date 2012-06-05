/****************************************************************************\
**	pqtControlUtil.hpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtControlUtil.hpp"

#include "Core/App/appTime.hpp"


#ifdef USE_QT

//============================================================================
//============================================================================
namespace
{
	undoUndoOperation *l_pLastOperation = NULL;
	pqtControl* l_pLastControl = NULL;
	float l_LastTime = 0.0f;

	const float c_TimeDiffThreshold = 1.0f; // Time difference in seconds to represent a new undo operation
}


//----------------------------------------------------------------------------
//	Checks if a new undo operation is needed, or if an old one can be
//	continued. It will either submit the undo operation or delete it.
//----------------------------------------------------------------------------
void pqtControlUtil::SubmitOrContinueUndo(pqtControl* i_pControl,
										  undoUndoOperation *i_pOperation)
{
	if (!i_pOperation) return;

	float cur_time = appTime::GetTime();

	// If the last operation was from the same control, try to continue using
	// the last undo operation
	if (l_pLastControl == i_pControl)
	{
		// If the last operation we submitted is still the last operation
		// on the stack
		if (undoUndoMgr::IsInLastOperation(l_pLastOperation))
		{
			// If the time between the last operation and the current one
			// is small, then keep using the old one
			if (cur_time - l_LastTime < c_TimeDiffThreshold)
			{
				// Since we are not submitting the operation, 
				// we have to delete it
				delete i_pOperation;

				// update the last time so that we can tell when long continuous 
				// changes to the same property last more than the threshold time
				l_LastTime = cur_time;
				return;
			}
		}
	}

	// It was decided that a new operation is needed, so submit it
	// to the undo manager
	undoUndoMgr::AddOperation(i_pOperation);

	// Remember the details of the operation we are submitting
	l_pLastOperation = i_pOperation;
	l_pLastControl = i_pControl;
	l_LastTime = cur_time;
}
#endif

