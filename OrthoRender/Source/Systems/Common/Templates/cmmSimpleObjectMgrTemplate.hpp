/*****************************************************************************
**	cmmSimpleObjectMgrTemplate.hpp
**
**	Template for object managers that maintain a simple list of objects
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SIMPLEOBJECTMGRTEMPLATE_HPP
#error cmmSimpleObjectMgrTemplate.hpp multiply included
#endif
#define CMM_SIMPLEOBJECTMGRTEMPLATE_HPP

#ifndef DBG_ASSERT_HPP
#include "Core/dbg/dbgAssert.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "Core/dbg/dbgLog.hpp"
#endif
#ifndef FS_FILEX_HPP
#include "Core/fs/fsFileX.hpp"
#endif
#ifndef FS_RESOURCETRACKERDATA_HPP
#include "Core/fs/fsResourceTrackerData.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif
#ifndef GUI_MESSAGEBOX_HPP
#include "Tool/gui/guiMessageBox.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef SEL3D_MGR_HPP
#include "Tool/sel3d/sel3dMgr.hpp"
#endif

#include <vector>


//============================================================================
//	forward references
//============================================================================


//============================================================================
// cmmSimpleObjectMgrTemplate defines the implementation of an object
//	manager for a system that contains a list of named property objects.
//	This system would not support scripting.
//============================================================================
template<class xxxObjectType, 
		 class xxxListData, 
		 class xxxItemData, 
		 class xxxObjectCreator>
class cmmSimpleObjectMgrTemplate
{
public:
	//--------------------------------------------------------------------
	// Update individual object data
	//--------------------------------------------------------------------
	static void SetData(int i_Index, const xxxItemData& i_Data)
	{
		sm_Objects[i_Index]->SetData(i_Data);
	}
	static xxxItemData GetData(int i_Index)
	{
		return sm_Objects[i_Index]->GetData();
	}

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	static xxxListData GetData()
	{
		xxxListData data;
		const int num_objects = sm_Objects.size();
		for (int i=0; i<num_objects; i++)
		{
			data.m_Items.push_back( sm_Objects[i]->GetData() );
		}
		return data;
	}

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const xxxListData &i_Data)
	{
		envSTLHelpers::DeleteContainer(sm_Objects);
		const int num_objects = i_Data.m_Items.size();
		for (int i=0; i<num_objects; i++)
		{
			create_and_add_object( i_Data.m_Items[i] );
		}
	}

	//--------------------------------------------------------------------
	// Append objects from this data into list, not allowing duplicates
	//--------------------------------------------------------------------
	static void MergeData(const xxxListData &i_Data)
	{
		const int num_items = i_Data.m_Items.size();
		for (int i=0; i<num_items; i++)
		{
			bool bDuplicate = false;
			const int num_objects = sm_Objects.size();
			for (int j=0; j<num_objects; j++)
			{
				if ( i_Data.m_Items[i].m_Name.GetString() == 
						sm_Objects[j]->GetName().GetString() )
				{
					// found a duplicate name, don't add this object
					bDuplicate = true;
					break;
				}
			
			}
			if (!bDuplicate)
			{
				create_and_add_object( i_Data.m_Items[i] );
			}
		}
	}

	//--------------------------------------------------------------------
	//  Add new object to world
	//--------------------------------------------------------------------
	static int  AddObject(const xxxItemData& i_Data)
	{
		try
		{
			create_and_add_object( i_Data );
		}
		catch( fsFileDoesntExistX& i_Ex )
		{
			char msg[256];
			std::string txt;
			txt = itStringUtil::GetStdString(i_Ex.GetLocator().GetLastName());
			sprintf( msg, "Cannot add the object -- missing %s", txt.c_str() );
			DBG_ERROR1( "%s", msg );
			guiMessageBox::Show(msg,"Error Loading Object", guiMessageBox::e_OKOnly);

			return -1;
			//assert(false);
		}

		int index = sm_Objects.size() - 1;
//		SelectObject(index);
		return index;
	}

	//--------------------------------------------------------------------
	//  Select object with given index
	//--------------------------------------------------------------------
	static void  SelectObject(int i_Index, bool i_bAppend = false)
	{
		if (sel3dMgr::GetSelected() != sm_Objects[i_Index])
		{
			sel3dMgr::CreateUndoOperation();
			if (i_bAppend)
				sel3dMgr::AddToSelection(sm_Objects[i_Index]);
			else
				sel3dMgr::Select(sm_Objects[i_Index]);
		}
	}

	//--------------------------------------------------------------------
	//  Remove object with given index from selection
	//--------------------------------------------------------------------
	static void  DeselectObject(int i_Index)
	{
		//if (sel3dMgr::GetSelected() != sm_Objects[i_Index])
		{
			sel3dMgr::CreateUndoOperation();
			sel3dMgr::RemoveFromSelection(sm_Objects[i_Index]);
		}
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	static void  DeleteObject(int i_Index)
	{
		// Note: could use a function here in xxxObjectCreator in order to 
		// do anything before deletion.
		sel3dMgr::ClearSelection();

		delete sm_Objects[i_Index];
		sm_Objects.erase(sm_Objects.begin() + i_Index);
	}

	//--------------------------------------------------------------------
	// return index of given object, returns -1 if not found
	//--------------------------------------------------------------------
	static int GetIndexForObject(xxxObjectType* i_pObject)
	{
		const int num_objects = sm_Objects.size();
		for (int i=0; i<num_objects; i++)
		{
			if (sm_Objects[i] == i_pObject)
			{
				return i;
			}
		}
		return -1;
	}

	//--------------------------------------------------------------------
	// return index of given object Name, returns -1 if not found
	//--------------------------------------------------------------------
	static int GetIndexForObject( const nameString& i_Name )
	{
		const int num_objects = sm_Objects.size();
		for (int i=0; i<num_objects; i++)
		{
			if (sm_Objects[i]->GetName() == i_Name)
			{
				return i;
			}
		}
		return -1;
	}
	//--------------------------------------------------------------------
	//  Return number of objects
	//--------------------------------------------------------------------
	static int GetNumObjects()
	{
		return sm_Objects.size();
	}

	//--------------------------------------------------------------------
	//  Get the script object pointer by index
	//--------------------------------------------------------------------
	static xxxObjectType* GetObject(int i_Index)
	{
		DBG_ASSERT0( (i_Index >= 0 && i_Index < (int) sm_Objects.size()), "index out of range" );
		return sm_Objects[i_Index];
	}

	//--------------------------------------------------------------------
	//  In simple object manager, the object is also the pick object
	//--------------------------------------------------------------------
	static xxxObjectType* GetPickObject(int i_Index)
	{
		return GetObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  clear
	//--------------------------------------------------------------------
	static void Clear()
	{
		sel3dMgr::ClearSelection();
		envSTLHelpers::DeleteContainer(sm_Objects);
	}

	//--------------------------------------------------------------------
	//  get a list of resources.  the resources will be appended to the
	//	passed in list.
	//--------------------------------------------------------------------
	static void GetResourceList( fsResourceTrackerData& io_List )
	{
		const int num_objects = sm_Objects.size();
		for (int i=0; i < num_objects; i++)
		{
			sm_Objects[i]->GetResourceList( io_List );
		}
	}

	//--------------------------------------------------------------------
	//	return true if the filename is NOT a dupe
	//--------------------------------------------------------------------
	static bool  VerifyNodupeName( char* i_Name )
	{
		const int num_objects = sm_Objects.size();
		for (int i=0; i<num_objects; i++)
		{
			if ( sm_Objects[i]->GetName().GetString() == i_Name )
			{
				return false;
			}
		}
		return true;
	}

protected:
	static std::vector<xxxObjectType*> sm_Objects;

		
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void create_and_add_object(const xxxItemData &i_Data)
	{
		xxxObjectType *pObject = xxxObjectCreator::Create(i_Data);
		sm_Objects.push_back(pObject);
	}

};	// end of static class


//============================================================================
// Initialiazing static members
//============================================================================
template<class xxxObjectType, class xxxListData, class xxxItemData, class xxxObjectCreator>
std::vector<xxxObjectType*> cmmSimpleObjectMgrTemplate<xxxObjectType,xxxListData,xxxItemData,xxxObjectCreator>::sm_Objects;
