/*****************************************************************************
**	grpsGroupMgr.cpp
**
**	Keeps track of groups of objects that can then have active and
**	draw style state altered as a group.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/grps/grpsGroupMgr.hpp"

#include "Support/grps/grpsGroupData.hpp"
#include "Support/grps/grpsGroupInterest.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

#include <list>
#include <set>


//============================================================================
//============================================================================
namespace grpsGroupMgr
{
	namespace
	{
		std::vector<grpsGroupInterest*>	l_InterestList;

		bool l_bDisableNotify = false;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void notify_interests_changed()
		{
			if (l_bDisableNotify) return;
			std::vector<grpsGroupInterest*>::iterator it, end = l_InterestList.end();
			for (it  = l_InterestList.begin(); it != end; ++it)
			{
				(*it)->DataChanged();
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//void notify_interests_added()
		//{
		//	if (l_bDisableNotify) return;
		//	std::vector<grpsGroupInterest*>::iterator it, end = l_InterestList.end();
		//	for (it  = l_InterestList.begin(); it != end; ++it)
		//	{
		//		(*it)->ObjectAdded();
		//	}
		//}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//void notify_interests_renamed()
		//{
		//	if (l_bDisableNotify) return;
		//	std::vector<grpsGroupInterest*>::iterator it, end = l_InterestList.end();
		//	for (it  = l_InterestList.begin(); it != end; ++it)
		//	{
		//		(*it)->ObjectRenamed();
		//	}
		//}

	//====================================================================
	// forward declaration
	//====================================================================
		class sGroup;

		//====================================================================
		//====================================================================
		class sObject
		{
		public:
			nameObject*			m_pNameObject;
			sel3dObject*	m_pObject;
			std::set<sGroup*> m_ContainingGroups;

			//----------------------------------------------------------------
			//----------------------------------------------------------------
			sObject(nameObject* i_pNameObj, sel3dObject* i_pObject)
			:	m_pNameObject(i_pNameObj), 
				m_pObject(i_pObject) 
			{}

			//----------------------------------------------------------------
			//----------------------------------------------------------------
			~sObject() 
			{
				m_pObject = NULL;
				m_pNameObject = NULL;
				//m_ContainingGroups.clear();
				//envSTLHelpers::DeleteContainer(m_ContainingGroups);
			}
		};

			// Note: It may have been easier to use std::map here, but I wasn't sure if
			// we could guarantee that the names were unique. [ba]
		std::vector<sObject*> l_Objects;

		//====================================================================
		//====================================================================
		class sGroup
		{
		public:
			nameString m_Name;
			std::vector<sObject*> m_Objects;

			//----------------------------------------------------------------
			//----------------------------------------------------------------
			sGroup(const nameString& i_Name)
			:	m_Name(i_Name)
			{
			}

			//----------------------------------------------------------------
			//----------------------------------------------------------------
			~sGroup() 
			{
				//m_Objects.clear();
				//envSTLHelpers::DeleteContainer(m_Objects);
			}
		};

		std::vector<sGroup*> l_Groups;


		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		template<class S> 
		class name_search
		{
		public:
			name_search(const nameString& i_Name) : m_Name(i_Name) {};
			bool operator () ( S* i_Set )
			{
				return (m_Name == i_Set->m_Name);
			}
			nameString m_Name;
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		template<class S> 
		class nameobj_search
		{
		public:
			nameobj_search(const nameString& i_Name) : m_Name(i_Name) {};
			bool operator () ( S* i_Set )
			{
				if (i_Set->m_pNameObject != 0)
                    return (m_Name == i_Set->m_pNameObject->GetName());
				else
					return false;
			}
			nameString m_Name;
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		template<class S> 
		class objptr_search
		{
		public:
			objptr_search(nameObject* i_pObject) : m_pObject(i_pObject) {};
			bool operator () ( S* i_Set )
			{
				return (m_pObject == i_Set->m_pNameObject);
			}
			nameObject* m_pObject;
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		template<class S> 
		class pickobjptr_search
		{
		public:
			pickobjptr_search(sel3dObject* i_pObject) : m_pObject(i_pObject) {};
			bool operator () ( S* i_Set )
			{
				return (m_pObject == i_Set->m_pObject);
			}
			sel3dObject* m_pObject;
		};

	}	// end of namespace


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Initialize()
	{
		// We could alternatively create an "unassigned" group with
		// an empty name. Then all objects that are not in a set would belong
		// to that sGroup.  So far, I don't see that to be an advantage.
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeInitialize()
	{
		Clear(false);
	}

	//--------------------------------------------------------------------
	//	Clear all groups and objects
	//--------------------------------------------------------------------
	void Clear(bool i_bNotifyInterests)
	{
		// Clean up
		envSTLHelpers::DeleteContainer(l_Groups);
		envSTLHelpers::DeleteContainer(l_Objects);

		if (i_bNotifyInterests)
			notify_interests_changed();
	}

	//------------------------------------------------------------------------
	//  Add named object to list of things that can be grouped
	//------------------------------------------------------------------------
	void  AddObject(nameObject* i_pNameObj, 
				    sel3dObject* i_pObject)
	{
		//if (i_pNameObj == NULL)
		//	return;
		//if (i_pObject == NULL)
		//	return;

		l_Objects.push_back(new sObject(i_pNameObj,i_pObject));
		
		notify_interests_changed();		//? notify_interests_added()
	}

	//--------------------------------------------------------------------
	//	Remove object from manager  (removing from all groups)
	//--------------------------------------------------------------------
	void RemoveObject(nameObject* i_pNameObj, 
					  sel3dObject* i_pObject)
	{
		if (i_pNameObj == NULL)
			return;
		if (i_pObject == NULL)
			return;

		//	print out the name list (DEBUG ONLY)
		//DBG_LOG("NAMES");
		//nameList aNameList;
		//nameMgr::GetNameList( aNameList );
		//for ( int i = 0 ; i < aNameList.size() ; i++ )
		//{
		//	DBG_LOG3( "%02d) %d %s", i, aNameList[i]->GetUID(), aNameList[i]->GetString().c_str() );
		//}
		//DBG_LOG("-----------------------------------------------");

		// Find named object
		// (A name search is not enough here, needs to use object pointer)
		//
		//DBG_LOG("Remove Object (" << i_pNameObj->GetName().GetString().c_str() << ")" );
		std::vector<sObject*>::iterator obj_it, obj_end = l_Objects.end();
		for (obj_it = l_Objects.begin(); obj_it != obj_end; ++obj_it)
		{
			sObject* pObject = (*obj_it);
			//DBG_LOG("   Object (" << pObject->m_pNameObject->GetName().GetString().c_str() << ")" );

			if (i_pNameObj == pObject->m_pNameObject)
			{
				//DBG_LOG("      Match!");

				//	Found the name, so now remove from containing groups
				//
				std::set<sGroup*>::iterator set_it, set_end = pObject->m_ContainingGroups.end();
				for (set_it = pObject->m_ContainingGroups.begin(); set_it != set_end; ++set_it)
				{
					sGroup* pGroup = (*set_it); 
					//int obj_count = pGroup->m_Objects.size();

					//	search the objects of this group and see if there is a match.
					//
					bool found = envSTLHelpers::RemoveOneValue(pGroup->m_Objects, pObject);
					if (found)
					{
						//DBG_LOG("      Group = " << pGroup->m_Name.GetString().c_str());
					}
				}

				//	after going through all the groups and removing the references, jump out
				break;
			}
		}

		// If the object was found, delete the object and remove the 
		// iterator from the container.
		//	[NOTE: this was moved out of above loop because it caused problems in VS2005
		//
		if (obj_it != obj_end)
		{
			//DBG_LOG("   DELETED");
			sObject* pObject = (*obj_it);
			l_Objects.erase(obj_it);
			delete pObject;
		}

		notify_interests_changed();		//? notify_interests_added()
	}

	//------------------------------------------------------------------------
	//	Change assignments from OldNameObj to NewNameObj (both of which
	//	should be registered with the manager at the point of calling 
	//	this function). This is used when reloading or replacing 
	//	geometry in a system.
	//------------------------------------------------------------------------
	void ReplaceObject(nameObject* i_pOldObject, 
					   nameObject* i_pNewObject)
	{
		if (i_pOldObject == NULL)
			return;
		if (i_pNewObject == NULL)
			return;

		// Find named objects (using pointer, since they both have same nameString)
		std::vector<sObject*>::iterator obj1_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), objptr_search<sObject>(i_pOldObject));
		std::vector<sObject*>::iterator obj2_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), objptr_search<sObject>(i_pNewObject));

		if (obj1_it != l_Objects.end() && obj2_it != l_Objects.end())
		{
			std::set<sGroup*>::iterator set_it, set_end = (*obj1_it)->m_ContainingGroups.end();
			for (set_it = (*obj1_it)->m_ContainingGroups.begin(); set_it != set_end; ++set_it)
			{
				sGroup* pSet = (*set_it); 

				// Add new object to group
				pSet->m_Objects.push_back(*obj2_it);
				(*obj2_it)->m_ContainingGroups.insert(pSet);
			}
		}
	}

	//------------------------------------------------------------------------
	//	Notify the group manager that the name of this object has changed
	//------------------------------------------------------------------------
	void ObjectRenamed(nameObject* i_pNameObj)
	{
		notify_interests_changed();		//? notify_interests_renamed();
	}


	//------------------------------------------------------------------------
	// Returns true if non-empty and unique name for a group.
	//------------------------------------------------------------------------
	bool IsValidGroupName(const std::string &i_Name)
	{
		if (i_Name.empty()) return false;

		std::vector<sGroup*>::iterator it, end = l_Groups.end();
		for (it = l_Groups.begin(); it != end; ++it)
		{
			if ((*it)->m_Name.GetString() == i_Name)
				return false;
		}
		return true;
	}

	//------------------------------------------------------------------------
	//	Create a named group
	//------------------------------------------------------------------------
	grpsGroupHandle  CreateGroup(const nameString& i_Name, bool i_bNotifyInterests )
	{
		DBG_ASSERT(!i_Name.IsEmpty(), "A group needs a valid name");

		// Check for unique name, GUI should use IsValidGroupName
		std::vector<sGroup*>::iterator group_it = 
			std::find_if(l_Groups.begin(), l_Groups.end(), name_search<sGroup>(i_Name));
		if (group_it != l_Groups.end())
		{
			DBG_WARNING("Selection set names should be unique: " << i_Name.GetString().c_str());
		}

		sGroup *pGroup = new sGroup(i_Name);
		l_Groups.push_back(pGroup);
		nameMgr::RegisterName( pGroup->m_Name );

		if (i_bNotifyInterests)
			notify_interests_changed();

		return reinterpret_cast<grpsGroupHandle>(pGroup);
	}

	
	//--------------------------------------------------------------------
	//	Return objects registered with group manager that are
	//	currently selected.
	//--------------------------------------------------------------------
	void GetSelectedGroupObjects(std::vector<nameString> &o_ObjectNames)
	{
		const std::list<sel3dObject*>& selected_list = sel3dMgr::GetSelectedList();
		//DBG_LOG("GetSelectedGroupObjects - selection list has " << selected_list.size() << " items" );

		std::list<sel3dObject*>::const_iterator it, end = selected_list.end();
		for (it = selected_list.begin(); it != end; ++it)
		{
			// Find registered object
			std::vector<sObject*>::iterator obj_it = 
				std::find_if(l_Objects.begin(), l_Objects.end(), pickobjptr_search<sObject>(*it));
			if  (obj_it != l_Objects.end())
			{
				if ((*obj_it)->m_pNameObject != NULL)
				{
					//DBG_LOG("GetSelectedGroupObjects - adding object:  " << (*obj_it)->m_pNameObject->GetName().GetString().c_str());
					o_ObjectNames.push_back( (*obj_it)->m_pNameObject->GetName() );
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//	Create a named group from the sel3dMgr selected list
	//--------------------------------------------------------------------
	grpsGroupHandle CreateGroupFromSelected( const nameString& i_Name )
	{
		grpsGroupHandle group_handle = CreateGroup(i_Name, false);

		//	loop through the selected items and build a single form
		//
		const std::list<sel3dObject*>& selected_list = sel3dMgr::GetSelectedList();
		//DBG_LOG("Create Group from selected list of " << selected_list.size() << " items" );

		std::list<sel3dObject*>::const_iterator it, end = selected_list.end();
		for (it = selected_list.begin(); it != end; ++it)
		{
			//if (nameObject* pObject = dynamic_cast<nameObject*>(*it))
			{
				AddObjectToGroup( i_Name, (*it), false );
			}
		}

		notify_interests_changed();

		return group_handle;
	}

	//--------------------------------------------------------------------
	//	Update a named group from the sel3dMgr selected list
	//--------------------------------------------------------------------
	void UpdateGroupFromSelected( const nameString& i_Name )
	{
		ClearGroup(i_Name, false);

		//	loop through the selected items and build a single form
		//
		const std::list<sel3dObject*>& selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator it, end = selected_list.end();
		for (it = selected_list.begin(); it != end; ++it)
		{
			//if (nameObject* pObject = dynamic_cast<nameObject*>(*it))
			{
				AddObjectToGroup( i_Name, (*it), false );
			}
		}

		notify_interests_changed();
	}

	//------------------------------------------------------------------------
	//	Destroy named group, all objects in this set become 
	//	"unassigned"
	//------------------------------------------------------------------------
	void  DeleteGroup(const nameString& i_Name)
	{
		// Find named group
		std::vector<sGroup*>::iterator group_it = 
			std::find_if(l_Groups.begin(), l_Groups.end(), name_search<sGroup>(i_Name));
		if  (group_it != l_Groups.end())
		{
			sGroup *pGroup = (*group_it);

			if (pGroup)
			{
				// Remove this group from the objects' containing groups
				std::vector<sObject*>::iterator obj_it, obj_end = pGroup->m_Objects.end();
				for (obj_it = pGroup->m_Objects.begin(); obj_it != obj_end; ++obj_it)
				{
					(*obj_it)->m_ContainingGroups.erase(pGroup);
				}
			}

			l_Groups.erase(group_it);
			delete pGroup;
		}

		notify_interests_changed();
	}

	//------------------------------------------------------------------------
	//	Destroy named group, using std::string
	//------------------------------------------------------------------------
	void  DeleteGroup(const std::string& i_Name)
	{
		std::vector<sGroup*>::iterator it, end = l_Groups.end();
		for (it = l_Groups.begin(); it != end; ++it)
		{
			sGroup *pGroup = (*it);
			if (pGroup->m_Name.GetString() == i_Name)
			{
				DeleteGroup(pGroup->m_Name);
				break;
			}
		}
	}

	//--------------------------------------------------------------------
	//	Clear named group of objects, but keep the group
	//--------------------------------------------------------------------
	void ClearGroup(const nameString& i_Name, bool i_bNotifyInterests )
	{
		std::vector<sGroup*>::iterator it, end = l_Groups.end();
		for (it = l_Groups.begin(); it != end; ++it)
		{
			sGroup *pGroup = (*it);
			if (pGroup->m_Name == i_Name)
			{
				// Remove this group from the objects' containing groups
				std::vector<sObject*>::iterator obj_it, obj_end = pGroup->m_Objects.end();
				for (obj_it = pGroup->m_Objects.begin(); obj_it != obj_end; ++obj_it)
				{
					(*obj_it)->m_ContainingGroups.erase(pGroup);
				}

				// Now, clear the list
				pGroup->m_Objects.clear();
				break;
			}
		}
	}

	//------------------------------------------------------------------------
	//	Rename a group
	//------------------------------------------------------------------------
	void  RenameGroup(const nameString& i_OldName, const nameString& i_NewName)
	{
		std::vector<sGroup*>::iterator it, end = l_Groups.end();
		for (it = l_Groups.begin(); it != end; ++it)
		{
			sGroup *pGroup = (*it);
			if (pGroup->m_Name == i_OldName)
			{
				pGroup->m_Name = i_NewName;
				notify_interests_changed();
				break;
			}
		}

		//notify_interests_added();
	}

	//------------------------------------------------------------------------
	//	Select a group
	//------------------------------------------------------------------------
	void  SelectGroupObjects(const nameString& i_GroupName)
	{
		std::vector<sGroup*>::iterator group_it = 
			std::find_if(l_Groups.begin(), l_Groups.end(), name_search<sGroup>(i_GroupName));
		if  (group_it != l_Groups.end())
		{
			// By creating the one undo operation at the beginning, the
			// whole selection change becomes a single operation
			sel3dMgr::CreateUndoOperation();
			sel3dMgr::ClearSelection();

			sGroup *pGroup = (*group_it);
			std::vector<sObject*>::iterator it, end = pGroup->m_Objects.end();
			for (it = pGroup->m_Objects.begin(); it != end; ++it)
			{
				if ((*it)->m_pObject != 0)
				{
					sel3dMgr::AddToSelection( (*it)->m_pObject  );
				}
			}

			//
			//int ns = sel3dMgr::GetNumSelected();
			//DBG_LOG("selected " << ns << " group objects" );
		}
	}

	//------------------------------------------------------------------------
	// Remove all groups (preparing for a new scene)
	//------------------------------------------------------------------------
	void ClearAllGroups()
	{
		// Disconnect all groups before destroying
		std::vector<sGroup*>::iterator it, end = l_Groups.end();
		for (it = l_Groups.begin(); it != end; ++it)
		{
			sGroup *pGroup = (*it);

			// TODO - anything to do here? the destructor takes care of deleting all the groups + objects
		}

		//	delete all the groups
		envSTLHelpers::DeleteContainer(l_Groups);

		notify_interests_changed();
	}

	//------------------------------------------------------------------------
	//	Add object to the given group
	//------------------------------------------------------------------------
	void  AddObjectToGroup(	const nameString& i_GroupName,
							sel3dObject* i_pPickObject, 
							bool i_bNotifyInterests )
	{
		// Find named object
		std::vector<sObject*>::iterator obj_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), pickobjptr_search<sObject>(i_pPickObject));
		// Find named group
		std::vector<sGroup*>::iterator group_it = 
			std::find_if(l_Groups.begin(), l_Groups.end(), name_search<sGroup>(i_GroupName));
		if  (group_it != l_Groups.end())
		{
			sGroup *pGroup = (*group_it);

			if  (obj_it != l_Objects.end())
			{
				// Add object to light set
				pGroup->m_Objects.push_back(*obj_it);
				(*obj_it)->m_ContainingGroups.insert(pGroup);
			}
		}

		if (i_bNotifyInterests)
			notify_interests_changed();
	}

	//--------------------------------------------------------------------
	//	Add object to the given group
	//--------------------------------------------------------------------
	void  AddObjectToGroup(	const nameString& i_GroupName, 
							const nameString& i_ObjectName, 
							bool i_bNotifyInterests )
	{
		// Find named object
		std::vector<sObject*>::iterator obj_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobj_search<sObject>(i_ObjectName));
		// Find named group
		std::vector<sGroup*>::iterator group_it = 
			std::find_if(l_Groups.begin(), l_Groups.end(), name_search<sGroup>(i_GroupName));
		if  (group_it != l_Groups.end())
		{
			sGroup *pGroup = (*group_it);

			if  (obj_it != l_Objects.end())
			{
				// Add object to light set
				pGroup->m_Objects.push_back(*obj_it);
				(*obj_it)->m_ContainingGroups.insert(pGroup);
			}
		}

		if (i_bNotifyInterests)
			notify_interests_changed();
	}

	//------------------------------------------------------------------------
	//	Remove object from the given group
	//------------------------------------------------------------------------
	void  RemoveObjectFromGroup(const nameString& i_GroupName, 
							   const nameString& i_ObjectName, 
							   bool i_bNotifyInterests )
	{
		// Find named object
		std::vector<sObject*>::iterator obj_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobj_search<sObject>(i_ObjectName));
		// Find named group
		std::vector<sGroup*>::iterator group_it = 
			std::find_if(l_Groups.begin(), l_Groups.end(), name_search<sGroup>(i_GroupName));
		if  (group_it != l_Groups.end() && obj_it != l_Objects.end())
		{
			sGroup *pGroup = (*group_it);
			sObject *pObject = (*obj_it);

			// Remove object from group
			envSTLHelpers::RemoveOneValue(pGroup->m_Objects, pObject);
			pObject->m_ContainingGroups.erase(pGroup);
		}

		if (i_bNotifyInterests)
			notify_interests_changed();
	}

	//--------------------------------------------------------------------
	//  Convert from handle to name
	//--------------------------------------------------------------------
	const nameString& GetGroupName(grpsGroupHandle i_Handle)
	{
		sGroup *pGroup = reinterpret_cast<sGroup*>(i_Handle);
		return pGroup->m_Name;

	}
	void SetGroupName(grpsGroupHandle i_Handle, const nameString& i_Name)
	{
		sGroup *pGroup = reinterpret_cast<sGroup*>(i_Handle);
		pGroup->m_Name = i_Name;
	}

	//------------------------------------------------------------------------
	// Return number of groups
	//------------------------------------------------------------------------
	int GetNumGroups()
	{
		return l_Groups.size();
	}

	//------------------------------------------------------------------------
	//  Get names of groups
	//------------------------------------------------------------------------
	void GetGroupNames(std::vector<nameString> &o_Names)
	{
		std::vector<sGroup*>::iterator it, end = l_Groups.end();
		for (it = l_Groups.begin(); it != end; ++it)
		{
			o_Names.push_back((*it)->m_Name);		
		}
	}

	//------------------------------------------------------------------------
	//  Get names of objects in a group. Use empty
	//	i_GroupName ("") to ask about "unassigned" objects.
	//------------------------------------------------------------------------
	void GetObjectsInGroup(const nameString& i_GroupName, 
						 std::vector<nameString> &o_ObjectNames)
	{
		// Handle "unassigned" group
		if (i_GroupName.IsEmpty())
		{
			std::vector<sObject*>::iterator it, end = l_Objects.end();
			for (it = l_Objects.begin(); it != end; ++it)
			{
				// If vector of containing groups is empty, this object is unassigned
				if ((*it)->m_ContainingGroups.empty())
				{
					if ((*it)->m_pNameObject != 0)
						o_ObjectNames.push_back((*it)->m_pNameObject->GetName());
				}
			}
		}
		else
		{
			// Find named group
			std::vector<sGroup*>::iterator group_it = 
				std::find_if(l_Groups.begin(), l_Groups.end(), name_search<sGroup>(i_GroupName));
			if  (group_it != l_Groups.end())
			{
				sGroup *pGroup = (*group_it);

				o_ObjectNames.resize(pGroup->m_Objects.size());
				int i=0;
				std::vector<sObject*>::iterator it, end = pGroup->m_Objects.end();
				for (it = pGroup->m_Objects.begin(); it != end; ++it)
				{
					if ((*it)->m_pNameObject != 0)
						o_ObjectNames[i++] = (*it)->m_pNameObject->GetName();
				}
			}
		}
	}

	//--------------------------------------------------------------------
	// Get groups that the object with this name belongs to.
	//--------------------------------------------------------------------
	void GetGroupsForObject(const nameString &i_ObjectName, 
							std::vector<nameString> &o_GroupNames)
	{
		// Find named object
		std::vector<sObject*>::iterator obj_it = 
			std::find_if(l_Objects.begin(), l_Objects.end(), nameobj_search<sObject>(i_ObjectName));
		if (obj_it != l_Objects.end())
		{
			sObject* pObject = (*obj_it);

			// Gather names of containing groups
			std::set<sGroup*>::iterator set_it, set_end = pObject->m_ContainingGroups.end();
			for (set_it = pObject->m_ContainingGroups.begin(); set_it != set_end; ++set_it)
			{
				sGroup* pGroup = (*set_it); 
				o_GroupNames.push_back( pGroup->m_Name );
			}
		}
	}

	//------------------------------------------------------------------------
	//  Get list of all names of objects registered in the manager
	//------------------------------------------------------------------------
	void GetAllObjects(std::vector<nameString> &o_ObjectNames)
	{
		std::vector<sObject*>::iterator it, end = l_Objects.end();
		for (it = l_Objects.begin(); it != end; ++it)
		{
			if ((*it)->m_pNameObject != 0)
				o_ObjectNames.push_back((*it)->m_pNameObject->GetName());
		}
	}
	
	//------------------------------------------------------------------------
	//  Access to whole data as one structure 
	//------------------------------------------------------------------------
	grpsGroupsData GetData()
	{
		grpsGroupsData groups_data;
		groups_data.m_Groups.resize(l_Groups.size());

		int si = 0;
		std::vector<sGroup*>::iterator it, end = l_Groups.end();
		for (it = l_Groups.begin(); it != end; ++it)
		{
			sGroup* pGroup = (*it);

			grpsGroupData &data = groups_data.m_Groups[si++];
			data.m_Name = pGroup->m_Name;	
			
			//DBG_LOG3("GetData: group: %s uid: %d, num objects %d", data.m_Name.GetString().c_str(), data.m_Name.GetUID(), pGroup->m_Objects.size());

			int i=0;
			data.m_ObjectNames.resize(pGroup->m_Objects.size());
			std::vector<sObject*>::iterator obj_it, obj_end = pGroup->m_Objects.end();
			for (obj_it = pGroup->m_Objects.begin(); obj_it != obj_end; ++obj_it)
			{
				//	if the object is a nameObject then store it.
				//
				// TODO figure out how to store non-named objects
				//
				if ((*obj_it)->m_pNameObject != 0)
				{
					nameString name = (*obj_it)->m_pNameObject->GetName();
					//DBG_LOG2("  Group object: %s uid %d", name.GetString().c_str(), name.GetUID());
					data.m_ObjectNames[i++] = name;
				}
			}
		}

		return groups_data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetData(const grpsGroupsData &i_Data)
	{
		l_bDisableNotify = true;
		ClearAllGroups();

		std::vector<grpsGroupData>::const_iterator it, end = i_Data.m_Groups.end();
		for (it = i_Data.m_Groups.begin(); it != end; ++it)
		{
			const grpsGroupData &data = (*it);

			//DBG_LOG3("SetData: creating group: %s uid: %d, num objects %d", data.m_Name.GetString().c_str(), data.m_Name.GetUID(), data.m_Objects.size());
			CreateGroup(data.m_Name);

			const int num_Objects = data.m_ObjectNames.size();
			for (int i=0; i<num_Objects; ++i)
			{
				AddObjectToGroup(data.m_Name, data.m_ObjectNames[i]);
			}
		}

		l_bDisableNotify = false;
		notify_interests_changed();
	}

	//------------------------------------------------------------------------
	//  Adds given data to exisiting data in manager
	//
	//	If i_bRenameDupes is true, the duplicate entry will be renamed
	//	and merged.  If it is false, the duplicate will NOT be merged.
	//--------------------------------------------------------------------
	void MergeData(const grpsGroupsData &i_Data, bool i_bRenameDupes)
	{
		l_bDisableNotify = true;

		std::vector<grpsGroupData>::const_iterator it, end = i_Data.m_Groups.end();
		for (it = i_Data.m_Groups.begin(); it != end; ++it)
		{
			const grpsGroupData &data = (*it);
			//DBG_LOG3("MergeData: adding group: %s uid: %d, num objects %d", data.m_Name.GetString().c_str(), data.m_Name.GetUID(), data.m_Objects.size());

			// Only create group name if it does not already exist.
			std::vector<sGroup*>::iterator group_it = 
				std::find_if(l_Groups.begin(), l_Groups.end(), name_search<sGroup>(data.m_Name));
			if (group_it == l_Groups.end() || i_bRenameDupes)
			{
				if (i_bRenameDupes)
				{
					//DBG_LOG("Creating new group, " << data.m_Name.GetString().c_str() );
					CreateGroup(data.m_Name);
				}
				else
				{
					//DBG_LOG("Creating new group, " << data.m_Name.GetString().c_str() );
					CreateGroup(data.m_Name);
				}
			}

			const int num_Objects = data.m_ObjectNames.size();
			for (int i=0; i<num_Objects; ++i)
			{
				AddObjectToGroup(data.m_Name, data.m_ObjectNames[i]);
			}
		}

		l_bDisableNotify = false;
		notify_interests_changed();
	}

	//------------------------------------------------------------------------
	//	RegisterGroupInterest() - add a Group interest 
	//------------------------------------------------------------------------
	void RegisterGroupInterest( grpsGroupInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Light Set Interest" );
		l_InterestList.push_back( i_pInterest );
	}

	//------------------------------------------------------------------------
	//	UnRegisterGroupInterest() - remove a Group interest 
	//
	//	Note: this will NOT delete the Group interest.  It is up to the
	//	registerer.
	//------------------------------------------------------------------------
	void UnRegisterGroupInterest( grpsGroupInterest* i_pInterest )
	{
		envSTLHelpers::RemoveOneValue( l_InterestList, i_pInterest );
	}

}	// end of namespace
