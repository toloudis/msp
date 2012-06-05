/****************************************************************************\
**	sel3dMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/sel3d/sel3dMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"
#include "Tool/sel3d/sel3dSelectInterest.hpp"
#include "Tool/sel3d/sel3dUndoOperation.hpp"


//============================================================================
//============================================================================
namespace
{
	// lists of interests
	std::vector<sel3dSelectInterest*>	l_SelectInterestList;

	// list of selected objects
	std::list<sel3dObject*> l_SelectedList;

	// list of things interesected by the pick ray (this is not the selected list)
	//pick3dPickList	l_PickList;
	//int	l_PickedIndex = 0;

	bool l_bSelectionLocked = false;

	//--------------------------------------------------------------------
	// get first object in list (most recently selected), this
	// position is treated specially.
	//--------------------------------------------------------------------
	sel3dObject* get_top_selection()
	{
		if (l_SelectedList.empty())
			return NULL;
		else
			return (*l_SelectedList.begin());
	}

	//--------------------------------------------------------------------
	// notify select interests that object is being added to selection
	//--------------------------------------------------------------------
	void notify_append(sel3dObject* i_pObject)
	{
		envSTLHelpers::ForAll(l_SelectInterestList,
			std::bind2nd(std::mem_fun(&sel3dSelectInterest::AddedToSelection),i_pObject));
	}

	//--------------------------------------------------------------------
	// notify select interests that object is being added to selection
	//--------------------------------------------------------------------
	void notify_remove(sel3dObject* i_pObject)
	{
		envSTLHelpers::ForAll(l_SelectInterestList,
			std::bind2nd(std::mem_fun(&sel3dSelectInterest::RemovedFromSelection),i_pObject));
	}

	//--------------------------------------------------------------------
	// notify select interests that selection changed
	//--------------------------------------------------------------------
	void notify_selection_changed()
	{
		envSTLHelpers::ForAll(l_SelectInterestList,
			std::mem_fun(&sel3dSelectInterest::SelectionChanged));
	}

	//--------------------------------------------------------------------
	// set_selection - shared code to set the given object
	//	to be the only selected object
	//--------------------------------------------------------------------
	void set_selection( sel3dObject* i_pObject )
	{
		// make copy of old list before clearing in order to notify 
		std::list<sel3dObject*> old_selection = l_SelectedList;

		// clear the actual selected list
		l_SelectedList.clear();

		// If this object was already in the selected list, then
		// we don't want to notify that it is being de-selected.
		// Remove it from the old selected list before notifying
		envSTLHelpers::RemoveOneValue(old_selection, i_pObject);

		// notify interests that the objects are being deselected one by one
		std::list<sel3dObject*>::iterator it, end = old_selection.end();
		for (it = old_selection.begin(); it != end; ++it)
		{
			notify_remove(*it);
		}

		// Add the new object into the list
		l_SelectedList.push_front(i_pObject);
		notify_append(i_pObject);
	}

	//--------------------------------------------------------------------
	// append_selection - shared code to add the given object
	//	to the front of the selection list
	//--------------------------------------------------------------------
	void append_selection( sel3dObject* i_pObject )
	{
		// If this object was already in the selected list, remove
		// it from the list without notifying. This will move it
		// to the front
		if (envSTLHelpers::RemoveOneValue(l_SelectedList, i_pObject))
		{
			// Add the new object into the list, but don't notify
			// because it was already in the list
			l_SelectedList.push_front(i_pObject);
		}
		else
		{
			// Add the new object into the list
			l_SelectedList.push_front(i_pObject);
			notify_append(i_pObject);
		}
	}
}


//--------------------------------------------------------------------
//	RegisterSelectInterest() - add a Select interest to the system
//--------------------------------------------------------------------
void sel3dMgr::RegisterSelectInterest( sel3dSelectInterest* i_pInterest )
{
	DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Select Interest" );

	l_SelectInterestList.push_back( i_pInterest );
}

//--------------------------------------------------------------------
//	UnRegisterSelectInterest() - remove a Select interest from the system.
//
//	Note: this will NOT delete the Select interest.  It is up to the
//	registerer.
//--------------------------------------------------------------------
void sel3dMgr::UnRegisterSelectInterest( sel3dSelectInterest* i_pInterest )
{
	envSTLHelpers::RemoveOneValue( l_SelectInterestList, i_pInterest );
}

//----------------------------------------------------------------------------
// Call this function to make the selection undoable before you change the
// Selection List
//----------------------------------------------------------------------------
void sel3dMgr::CreateUndoOperation()
{
	undoUndoMgr::AddOperation(new sel3dUndoOperation(l_SelectedList));
}

//----------------------------------------------------------------------------
//	Clear() - clear the list
//----------------------------------------------------------------------------
void sel3dMgr::Clear()
{
	ClearSelection();
	l_SelectInterestList.clear();
}

//--------------------------------------------------------------------
//	Renotify() - Notify the selection changed interests again, 
//   without changing the selection. 
//--------------------------------------------------------------------
void sel3dMgr::Renotify()
{
	notify_selection_changed();
}

//--------------------------------------------------------------------
//	ClearSelection() - clear the current selection and the
//	current pick object list
//--------------------------------------------------------------------
void sel3dMgr::ClearSelection()
{
	if(l_bSelectionLocked)
		return;
	// clear the pick list
	//l_PickList.Clear();
	//l_PickedIndex = 0;

	// make copy of old list before clearing in order to notify 
	std::list<sel3dObject*> old_selection = l_SelectedList;

	// clear the actual selected list
	l_SelectedList.clear();

	// notify interests that the objects are being deselected one by one
	std::list<sel3dObject*>::iterator it, end = old_selection.end();
	for (it = old_selection.begin(); it != end; ++it)
	{
		notify_remove(*it);
	}

	// now do more general selection changed callbacks once
	notify_selection_changed();
}

//--------------------------------------------------------------------
//	GetSelected() - gets the currently "top" selected item
//--------------------------------------------------------------------
sel3dObject* sel3dMgr::GetSelected()
{
	return get_top_selection();
}

//--------------------------------------------------------------------
// Select a specific object (without using a ray pick list)
//--------------------------------------------------------------------
void sel3dMgr::Select(sel3dObject* i_pObject)
{
	if(l_bSelectionLocked)
		return;
	if (i_pObject == NULL)
	{
		DBG_WARNING( "Selecting a NULL object -- clearing list" );
		ClearSelection();
		return;
	}

	// replace pick list with single object
	//l_PickList.Clear();
	//l_PickList.AddItem( i_pObject, 1.0f );
	//l_PickedIndex = 0;

	// update selection list
	set_selection(i_pObject);

	// now do more general selection changed callbacks once
	notify_selection_changed();
}

//--------------------------------------------------------------------
// Lock/Unlock the selection, stops anything from being added/removed
// on the pick object list
//--------------------------------------------------------------------
void sel3dMgr::ToggleSelectionLock()
{
	l_bSelectionLocked = !l_bSelectionLocked;
}

//--------------------------------------------------------------------
// get the selection lock flag
//--------------------------------------------------------------------
bool sel3dMgr::getSelectionLock()
{
	return l_bSelectionLocked;
}

//--------------------------------------------------------------------
// set the selection lock flag
//--------------------------------------------------------------------
void sel3dMgr::setSelectionLock(bool i_bLockSelection)
{
	l_bSelectionLocked = i_bLockSelection;
}

//--------------------------------------------------------------------
// Append specific object to selected list 
// (without using a ray pick list)
//--------------------------------------------------------------------
void sel3dMgr::AddToSelection(sel3dObject* i_pObject)
{
	if(l_bSelectionLocked)
		return;
	if (i_pObject == NULL)
	{
		DBG_WARNING("Trying to append a NULL object" );
		return;
	}

	// replace pick list with single object
	//l_PickList.Clear();
	//l_PickList.AddItem( i_pObject, 1.0f );
	//l_PickedIndex = 0;

	// update selection list
	append_selection(i_pObject);

	// now do more general selection changed callbacks once
	notify_selection_changed();
}

//--------------------------------------------------------------------
// Select a specific object from selected list 
// (without using a ray pick list)
//--------------------------------------------------------------------
void sel3dMgr::RemoveFromSelection(sel3dObject* i_pObject)
{
	if(l_bSelectionLocked)
		return;
	if (i_pObject == NULL)
	{
		DBG_WARNING("Trying to append a NULL object" );
		return;
	}

	// If this object was the top selection, then
	// we need to update the pick list
	bool bWasTopSelection = (get_top_selection() == i_pObject);

	// Remove object from list, notifying if found
	// If the object was not found, don't do anything else
	if (envSTLHelpers::RemoveOneValue(l_SelectedList, i_pObject))
	{
		notify_remove(i_pObject);


		// Update pick list if needed
		//if (bWasTopSelection) 
		//{
		//	l_PickList.Clear();
		//	l_PickedIndex = 0;

		//	if (!l_SelectedList.empty())
		//		l_PickList.AddItem( get_top_selection(), 1.0f );
		//}

		// now do more general selection changed callbacks once
		notify_selection_changed();
	}
}

//--------------------------------------------------------------------
//	GetSelectedList() - gets the list of selected objects
//--------------------------------------------------------------------
const std::list<sel3dObject*>& sel3dMgr::GetSelectedList()
{
	return l_SelectedList;
}

//--------------------------------------------------------------------
// Return number of objects currently in selection list.
//--------------------------------------------------------------------
int sel3dMgr::GetNumSelected()
{
	return l_SelectedList.size();
}

//--------------------------------------------------------------------
// Return true if this pick object is included in the selection list
//--------------------------------------------------------------------
bool sel3dMgr::IsSelected(sel3dObject* i_pPickObj)
{
	return (envSTLHelpers::Contains(l_SelectedList, i_pPickObj));
}

//--------------------------------------------------------------------
// GetPickList() - gets the list of objects who intersected
//  the last pick ray
//--------------------------------------------------------------------
//const pick3dPickList& sel3dMgr::GetPickList()
//{
//	return l_PickList;
//}

//--------------------------------------------------------------------
//	SetPickList - set the list of objects that intersected the 
//	pick ray. If i_bAppend is true, then it adds the first object
//	in the pick list to the selected list. If it is false, it
//	clears out the selected list and sets this object to be selected.
//--------------------------------------------------------------------
//void sel3dMgr::SetPickList( pick3dPickList& i_PickList, 
//						    bool i_bAppend /* = false */ )
//{
//	if ( i_PickList.GetSize() == 0 )
//	{
//		ClearSelection();
//		return;
//	}
//
//	// Set the new pick list
//	l_PickList.CopyFrom( i_PickList );
//	l_PickedIndex = 0;
//
//	sel3dObject* pObject = l_PickList.GetItem( 0 )->GetObject();
//
//	// If appending, add object to the selected list, otherwise
//	// replace the selected list with this object.
//	if (i_bAppend)
//		append_selection( pObject );
//	else
//		set_selection( pObject );
//
//	// now do more general selection changed callbacks once
//	notify_selection_changed();
//}

//--------------------------------------------------------------------
//	SetNextToSelected() - set the next item in the list to selected
//--------------------------------------------------------------------
//sel3dObject* sel3dMgr::SetNextToSelected(bool i_bWrapAround)
//{
//	if(l_bSelectionLocked)
//		return NULL;
//	if ( l_PickList.GetSize() == 0 )
//		return NULL;
//	if ( l_PickList.GetSize() == 1 )
//		return get_top_selection();
//
//	// Remove old selected object from selected list, notifying if found
//	sel3dObject* pOldSelection = get_top_selection();
//	if (envSTLHelpers::RemoveOneValue(l_SelectedList, pOldSelection))
//	{
//		notify_remove(pOldSelection);
//	}
//
//	// compute next index
//	int next_index = (l_PickedIndex + 1) % l_PickList.GetSize();
//	if (!i_bWrapAround)
//	{
//		if (next_index < l_PickedIndex)
//			next_index = l_PickedIndex;
//	}
//
//	sel3dObject* pObject = l_PickList.GetItem( next_index )->GetObject();
//	l_PickedIndex = next_index;
//	append_selection( pObject );
//
//	// now do more general selection changed callbacks once
//	notify_selection_changed();
//
//	return pObject;
//}

//--------------------------------------------------------------------
//	SetPrevToSelected() - set the Prev item in the list to selected
//--------------------------------------------------------------------
//sel3dObject* sel3dMgr::SetPrevToSelected(bool i_bWrapAround)
//{
//	if(l_bSelectionLocked)
//		return NULL;
//	if ( l_PickList.GetSize() == 0 )
//		return NULL;
//	if ( l_PickList.GetSize() == 1 )
//		return get_top_selection();
//
//	// Remove old selected object from selected list, notifying if found
//	sel3dObject* pOldSelection = get_top_selection();
//	if (envSTLHelpers::RemoveOneValue(l_SelectedList, pOldSelection))
//	{
//		notify_remove(pOldSelection);
//	}
//
//	// compute prev index
//	int prev_index = l_PickedIndex - 1;
//	if (prev_index < 0)
//	{
//		prev_index = (i_bWrapAround ? (l_PickList.GetSize() - 1) : 0);
//	}
//
//	sel3dObject* pObject = l_PickList.GetItem( prev_index )->GetObject();
//	l_PickedIndex = prev_index;
//	append_selection( pObject );
//
//	// now do more general selection changed callbacks once
//	notify_selection_changed();
//
//	return pObject;
//}

//--------------------------------------------------------------------
// Select the object with given index from the pick list
//--------------------------------------------------------------------
//sel3dObject* sel3dMgr::SelectIndexFromPickList(int i_Index)
//{
//	if(l_bSelectionLocked)
//		return NULL;
//	if ( l_PickList.GetSize() == 0 || i_Index >= l_PickList.GetSize())
//		return NULL;
//	if ( l_PickedIndex == i_Index || l_PickList.GetSize() == 1 )
//		return get_top_selection();
//
//	// Remove old selected object from selected list, notifying if found
//	sel3dObject* pOldSelection = get_top_selection();
//	if (envSTLHelpers::RemoveOneValue(l_SelectedList, pOldSelection))
//	{
//		notify_remove(pOldSelection);
//	}
//
//	sel3dObject* pObject = l_PickList.GetItem( i_Index )->GetObject();
//	l_PickedIndex = i_Index;
//	append_selection( pObject );
//
//	// now do more general selection changed callbacks once
//	notify_selection_changed();
//
//	return pObject;
//}

