/*****************************************************************************\
**	sel3dMgr.hpp
**
**		the selelection system manager.
**	This controls all the selection interests for the system.
**
**	There are three aspects of the current selection:
**		1) There is a list of currently selected objects. This list can be
**			iterated, cleared, appended to and removed from.
**		2) The last object selected in the list is specially selected,
**			called the "top selection", some application operations may 
**			only act on this object.
**		3) There is a list of pick objects that came from the last ray pick.
**			Only one of these objects is selected, (it will be the "top 
**			selection"). The user can toggle through this list, changing the
**			top selection. Do not confuse this list with the list of selected
**			objects described in #1.
**
**	StudioGPU
**	Copyright(C) 2003-6 - All Rights Reserved
\****************************************************************************/
#ifdef SEL3D_MGR_HPP
#error sel3dMgr.hpp multiply included
#endif
#define SEL3D_MGR_HPP

#include <list>


//============================================================================
//	Forward References
//============================================================================
class sel3dObject;
class pick3dPickList;
class sel3dSelectInterest;


//============================================================================
//============================================================================
namespace sel3dMgr
{
//========================================================================
//	select interest list functions
//========================================================================

	//--------------------------------------------------------------------
	//	RegisterSelectInterest() - add a Select interest to the system
	//--------------------------------------------------------------------
	void RegisterSelectInterest( sel3dSelectInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterSelectInterest() - remove a Select interest from the system.
	//
	//	Note: this will NOT delete the Select interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterSelectInterest( sel3dSelectInterest* i_pInterest );

	//----------------------------------------------------------------------------
	// Call this function to make the selection undoable before you change the
	// Selection List
	//----------------------------------------------------------------------------
	void CreateUndoOperation();

	//--------------------------------------------------------------------
	//	Clear() - clear the select interest list
	//--------------------------------------------------------------------
	void Clear();

	//--------------------------------------------------------------------
	//	Renotify() - Notify the selection changed interests again, 
	//   without changing the selection. 
	//--------------------------------------------------------------------
	void Renotify();

//========================================================================
//	select list functions with single object attitude. 
//========================================================================

	//--------------------------------------------------------------------
	//	ClearSelection() - clear the current selection and the
	//	current pick object list
	//--------------------------------------------------------------------
	void ClearSelection();

	//--------------------------------------------------------------------
	//	GetSelected() - gets the currently "top" selected item
	//--------------------------------------------------------------------
	sel3dObject* GetSelected();

	//--------------------------------------------------------------------
	// Select a specific object (without using a ray pick list)
	//--------------------------------------------------------------------
	void Select(sel3dObject* i_Object);

	//--------------------------------------------------------------------
	// Lock/Unlock the selection, stops anything from being added/removed
	// on the pick object list
	//--------------------------------------------------------------------
	void ToggleSelectionLock();

	//--------------------------------------------------------------------
	// get the selection lock flag
	//--------------------------------------------------------------------
	bool getSelectionLock();

	//--------------------------------------------------------------------
	// set the selection lock flag
	//--------------------------------------------------------------------
	void setSelectionLock(bool i_bLockSelection = true);


//========================================================================
//	select list functions with list attitude. 
//========================================================================

	//--------------------------------------------------------------------
	// Append specific object to selected list 
	// (without using a ray pick list)
	//--------------------------------------------------------------------
	void AddToSelection(sel3dObject* i_pObject);
	
	//--------------------------------------------------------------------
	// Select a specific object from selected list 
	// (without using a ray pick list)
	//--------------------------------------------------------------------
	void RemoveFromSelection(sel3dObject* i_pObject);

	//--------------------------------------------------------------------
	//	GetSelectedList() - gets the list of selected objects
	//--------------------------------------------------------------------
	const std::list<sel3dObject*>& GetSelectedList();

	//--------------------------------------------------------------------
	// Return number of objects currently in selection list.
	//--------------------------------------------------------------------
	int GetNumSelected();

	//--------------------------------------------------------------------
	// Return true if this pick object is included in the selection list
	//--------------------------------------------------------------------
	bool IsSelected(sel3dObject* i_pPickObj);

//========================================================================
//	pick list functions
//========================================================================

	//--------------------------------------------------------------------
	// GetPickList() - gets the list of objects who intersected
	//  the last pick ray
	//--------------------------------------------------------------------
	//const pick3dPickList& GetPickList();

	//--------------------------------------------------------------------
	//	SetPickList - set the list of objects that intersected the 
	//	pick ray. If i_bAppend is true, then it adds the first object
	//	in the pick list to the selected list. If it is false, it
	//	clears out the selected list and sets this object to be selected.
	//--------------------------------------------------------------------
	//void SetPickList( pick3dPickList& i_PickList, bool i_bAppend = false );

	//--------------------------------------------------------------------
	//	SetNextToSelected() - set the next item in the list to selected
	//--------------------------------------------------------------------
	//sel3dObject* SetNextToSelected(bool i_bWrapAround = true);

	//--------------------------------------------------------------------
	//	SetPrevToSelected() - set the Prev item in the list to selected
	//--------------------------------------------------------------------
	//sel3dObject* SetPrevToSelected(bool i_bWrapAround = true);

	//--------------------------------------------------------------------
	// Select the object with given index from the pick list
	//--------------------------------------------------------------------
	//sel3dObject* SelectIndexFromPickList(int i_Index);
};
