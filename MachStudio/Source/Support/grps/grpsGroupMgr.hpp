/*****************************************************************************
**	grpsGroupMgr.hpp
**
**	Keeps track of groups of objects.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef GRPS_GROUPMGR_HPP
#error grpsGroupMgr.hpp multiply included
#endif
#define GRPS_GROUPMGR_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class nameObject;
class grpsGroupsData;
class grpsGroupInterest;
class sel3dObject;

//============================================================================
// Handle to refer to layer
//============================================================================
typedef void* grpsGroupHandle;


//============================================================================
//============================================================================
namespace grpsGroupMgr
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize();

	//--------------------------------------------------------------------
	//	Clear all groups and objects
	//--------------------------------------------------------------------
	void Clear(bool i_bNotifyInterests = true);

//
// Systems should call these functions in order to submit
// and organize objects from different systems.
//

	//--------------------------------------------------------------------
	//  Add named object to list of things that can be grouped
	//--------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
				    sel3dObject* i_pObject);

	//--------------------------------------------------------------------
	//	Remove object from manager (removing from all groups)
	//--------------------------------------------------------------------
	void RemoveObject(nameObject* i_pNameObj, 
					  sel3dObject* i_pObject);

	//--------------------------------------------------------------------
	//	Change assignments from OldNameObj to NewNameObj (both of which
	//	should be registered with the manager at the point of calling 
	//	this function). This is used when reloading or replacing 
	//	geometry in a system.
	//--------------------------------------------------------------------
	void ReplaceObject(nameObject* i_pOldNameObj, 
					   nameObject* i_pNewNameObj);

	//--------------------------------------------------------------------
	//	Notify the group manager that the name of this object has changed
	//--------------------------------------------------------------------
	void ObjectRenamed(nameObject* i_pNameObj);


//
// The user interface can then manipulate the groupings with the
// following functions by using the names of the groups and objects
//

	//--------------------------------------------------------------------
	// Returns true if non-empty and unique name for a group.
	//--------------------------------------------------------------------
	bool IsValidGroupName(const std::string &i_Name);

	//--------------------------------------------------------------------
	//	Create a named group
	//--------------------------------------------------------------------
	grpsGroupHandle  CreateGroup(const nameString& i_Name, bool i_bNotifyInterests = true );

	//--------------------------------------------------------------------
	//	Return objects registered with group manager that are
	//	currently selected.
	//--------------------------------------------------------------------
	void GetSelectedGroupObjects(std::vector<nameString> &o_ObjectNames);

	//--------------------------------------------------------------------
	//	Create a named group from the sel3dMgr selected list
	//--------------------------------------------------------------------
	grpsGroupHandle CreateGroupFromSelected( const nameString& i_Name );

	//--------------------------------------------------------------------
	//	Update a named group from the sel3dMgr selected list
	//--------------------------------------------------------------------
	void UpdateGroupFromSelected( const nameString& i_Name );

	//--------------------------------------------------------------------
	//	Destroy named group, all objects in this set become 
	//	"unassigned"
	//--------------------------------------------------------------------
	void  DeleteGroup(const nameString& i_Name);

	//--------------------------------------------------------------------
	//	Destroy named group, using std::string
	//	Note: Try to use the nameString function.
	//--------------------------------------------------------------------
	void  DeleteGroup(const std::string& i_Name);

	//--------------------------------------------------------------------
	//	Clear named group of objects, but keep the group
	//--------------------------------------------------------------------
	void  ClearGroup(const nameString& i_Name, bool i_bNotifyInterests = true );

	//--------------------------------------------------------------------
	//	Rename a group
	//--------------------------------------------------------------------
	void  RenameGroup(const nameString& i_OldName, const nameString& i_NewName);

	//--------------------------------------------------------------------
	//	Select a group
	//--------------------------------------------------------------------
	void  SelectGroupObjects(const nameString& i_GroupName);

	//--------------------------------------------------------------------
	// Remove all groups (preparing for a new scene)
	//--------------------------------------------------------------------
	void ClearAllGroups();

	//--------------------------------------------------------------------
	//	Add object to the given group
	//--------------------------------------------------------------------
	void  AddObjectToGroup(	const nameString& i_GroupName, 
							sel3dObject* i_pPickObject, 
							bool i_bNotifyInterests = true );

	//--------------------------------------------------------------------
	//	Add object to the given group
	//--------------------------------------------------------------------
	void  AddObjectToGroup(	const nameString& i_GroupName, 
							const nameString& i_ObjectName, 
							bool i_bNotifyInterests = true );

	//--------------------------------------------------------------------
	//	Remove object from the given group
	//--------------------------------------------------------------------
	void  RemoveObjectFromGroup(const nameString& i_GroupName, 
								const nameString& i_ObjectName, 
								bool i_bNotifyInterests = true );


	//--------------------------------------------------------------------
	//  Convert from handle to name
	//--------------------------------------------------------------------
	const nameString& GetGroupName(grpsGroupHandle i_Handle);
	void SetGroupName(grpsGroupHandle i_Handle, const nameString& i_Name);

//--------------------------------------------------------------------
// These functions get the current state of the group groupings
// in order to be displayed to the user.
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	// Return number of groups
	//--------------------------------------------------------------------
	int GetNumGroups();

	//--------------------------------------------------------------------
	//  Get names of groups
	//--------------------------------------------------------------------
	void GetGroupNames(std::vector<nameString> &o_Names);

	//--------------------------------------------------------------------
	//  Get names of objects in a group. Use empty
	//	i_GroupName ("") to ask about "unassigned" objects.
	//--------------------------------------------------------------------
	void GetObjectsInGroup(const nameString& i_GroupName, 
						 std::vector<nameString> &o_ObjectNames);

	//--------------------------------------------------------------------
	// Get groups that the object with this name belongs to.
	//--------------------------------------------------------------------
	void GetGroupsForObject(const nameString &i_ObjectName, 
							std::vector<nameString> &o_GroupNames);

	//--------------------------------------------------------------------
	//  Get list of all names of objects registered in the manager
	//--------------------------------------------------------------------
	void GetAllObjects(std::vector<nameString> &o_ObjectNames);

	//--------------------------------------------------------------------
	//  Access to whole data as one structure 
	//--------------------------------------------------------------------
	grpsGroupsData GetData();
	void SetData(const grpsGroupsData &i_Data);

	//--------------------------------------------------------------------
	//	Adds given data to exisiting data in manager
	//
	//	If i_bRenameDupes is true, the duplicate entry will be renamed
	//	and merged.  If it is false, the duplicate will NOT be merged.
	//--------------------------------------------------------------------
	void MergeData(const grpsGroupsData &i_Data, bool i_bRenameDupes = false);


//--------------------------------------------------------------------
//	data changed interest functions
//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	//	RegisterGroupInterest() - add a Group interest 
	//--------------------------------------------------------------------
	void RegisterGroupInterest( grpsGroupInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterGroupInterest() - remove a Group interest 
	//
	//	Note: this will NOT delete the Group interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterGroupInterest( grpsGroupInterest* i_pInterest );

}	// end of namespace
