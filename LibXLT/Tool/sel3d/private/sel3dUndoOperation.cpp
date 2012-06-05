/*****************************************************************************
**	sel3dUndoOperation.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Tool/sel3d/sel3dUndoOperation.hpp"

#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/sel3d/sel3dObject.hpp"

#include "Core/Dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
namespace
{
	void set_selection(std::list<sel3dObject*>& i_SelList)
	{
		sel3dMgr::ClearSelection();

		std::list<sel3dObject*>::reverse_iterator it, end = i_SelList.rend();
		for (it = i_SelList.rbegin(); it != end; ++it)
		{
			sel3dMgr::AddToSelection( *it );
		}
	}

	void create_references(const std::list<sel3dObject*> &i_SelectionList,
						  std::list< shared_ptr<relObjectReference> > &o_References)
	{
		o_References.clear();
		std::list<sel3dObject*>::const_iterator it, end = i_SelectionList.end();
		for (it = i_SelectionList.begin(); it != end; ++it)
		{
			o_References.push_back( (*it)->CreateReferenceToSelf() );
		}
	}

	void resolve_references(const std::list< shared_ptr<relObjectReference> > &i_References,
							std::list<sel3dObject*>	&o_SelectionList)
	{
		std::list< shared_ptr<relObjectReference> >::const_iterator it, end = i_References.end();
		for (it = i_References.begin(); it != end; ++it)
		{
			relObject *pObject = (*it)->GetObject();
			if (pObject)
			{
				sel3dObject *pSelObject = dynamic_cast<sel3dObject*>(pObject);
				if (pSelObject)
					o_SelectionList.push_back( pSelObject );
				else
					DBG_WARNING("Selection undo: Resolved object is not a selectable type.");
			}
			else
				DBG_WARNING("Could not resolve reference in selection undo operation.");
		}
	}
}

//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
sel3dUndoOperation::sel3dUndoOperation(std::list<sel3dObject*>& i_SelList)
{
	create_references(i_SelList, m_Backup);
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string  sel3dUndoOperation::GetDisplayName()
{
	return "Selection";
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float  sel3dUndoOperation::GetMemoryUsage()
{
	return (sizeof(m_Backup) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void  sel3dUndoOperation::Undo()
{
	std::list<sel3dObject*> new_selection;
	resolve_references(m_Backup, new_selection);
	create_references(sel3dMgr::GetSelectedList(), m_Backup); // switch backup from undo to redo
	set_selection(new_selection);
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void  sel3dUndoOperation::Redo()
{
	std::list<sel3dObject*> new_selection;
	resolve_references(m_Backup, new_selection);
	create_references(sel3dMgr::GetSelectedList(), m_Backup); // switch backup from undo to redo
	set_selection(new_selection);
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void  sel3dUndoOperation::Commit()
{
	// nothing needed
}

//--------------------------------------------------------------------
//  Destroy is called on an operation when it has been undone
// and it can no longer be redone. This may happen after the
// history gets long enough or a new operation is made when
// its current state is "undone". The destructor will soon be
// called.
//--------------------------------------------------------------------
void  sel3dUndoOperation::Destroy()
{
	// nothing needed
}


