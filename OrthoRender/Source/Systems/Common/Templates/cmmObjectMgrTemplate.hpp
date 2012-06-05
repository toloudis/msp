/*****************************************************************************
**	cmmObjectMgrTemplate.hpp
**
**	Template for object managers that maintain a list of script objects
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_OBJECTMGRTEMPLATE_HPP
#error cmmObjectMgrTemplate.hpp multiply included
#endif
#define CMM_OBJECTMGRTEMPLATE_HPP

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
#ifndef GEO_PICKRAY_HPP
#include "Core/geo/geoPickRay.hpp"
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
#include <assert.h>


//============================================================================
//	forward references
//============================================================================
class geoPickRay;
class pick3dPickList;


//============================================================================
// cmmObjectMgrTemplateNoIcon defines the implementation of an object
//	manager for a system that contains a list of objects where each
//	object is split such that a script object contains a property object.
//============================================================================
template<class xxxScriptObject, 
		 class xxxPickObject, 
		 class xxxListData, 
		 class xxxScriptData, 
		 class xxxBaseData,
		 class xxxObjectCreator>
class cmmObjectMgrTemplateNoIcon
{
public:
	//--------------------------------------------------------------------
	// Update individual base data
	//--------------------------------------------------------------------
	static void SetBaseData(int i_Index, const xxxBaseData& i_Data)
	{
		sm_Objects[i_Index]->SetBaseData(i_Data);
	}
	static xxxBaseData GetBaseData(int i_Index)
	{
		return sm_Objects[i_Index]->GetBaseData();
	}

	//--------------------------------------------------------------------
	// Update individual object data
	//--------------------------------------------------------------------
	static void SetScriptData(int i_Index, const xxxScriptData& i_Data)
	{
		sm_Objects[i_Index]->SetScriptData(i_Data);
	}
	static xxxScriptData GetScriptData(int i_Index)
	{
		return sm_Objects[i_Index]->GetScriptData();
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
			data.m_Items.push_back( sm_Objects[i]->GetScriptData() );
		}
		return data;
	}

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetDataUnthreaded(const xxxListData &i_Data)
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
				if ( i_Data.m_Items[i].m_BaseData.m_Name.GetString() == 
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
	static int  AddObject(const xxxScriptData& i_Data)
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
		if (sel3dMgr::GetSelected() != sm_Objects[i_Index]->GetPickObject())
		{
			sel3dMgr::CreateUndoOperation();
			if (i_bAppend)
				sel3dMgr::AddToSelection(sm_Objects[i_Index]->GetPickObject());
			else
				sel3dMgr::Select(sm_Objects[i_Index]->GetPickObject());
		}
	}

	//--------------------------------------------------------------------
	//  Remove object with given index from selection
	//--------------------------------------------------------------------
	static void  DeselectObject(int i_Index)
	{
		//if (sel3dMgr::GetSelected() != sm_Objects[i_Index]->GetPickObject())
		{
			sel3dMgr::CreateUndoOperation();
			sel3dMgr::RemoveFromSelection(sm_Objects[i_Index]->GetPickObject());
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
	static int GetIndexForObject(xxxPickObject* i_pObject)
	{
		const int num_objects = sm_Objects.size();
		for (int i=0; i<num_objects; i++)
		{
			if (sm_Objects[i]->GetPickObject() == i_pObject)
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
			if (sm_Objects[i]->GetPickObject()->GetName() == i_Name)
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
	};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//static void GetObject(int i_Index)
	//{
	//};

	//--------------------------------------------------------------------
	//  Get the script object pointer by index
	//--------------------------------------------------------------------
	static xxxScriptObject* GetObjectFromIndex(int i_Index)
	{
//		DBG_ASSERT2( ((i_Index >= 0) && (i_Index < ((int) sm_Objects.size()))), "index (%d) out of range (%d)", i_Index, ((int)sm_Objects.size()) );
		return sm_Objects[i_Index];
	};

	//--------------------------------------------------------------------
	//  Get the script object pointer by index
	//--------------------------------------------------------------------
	static xxxScriptObject* GetObject(int i_Index)
	{
//		DBG_ASSERT2( ((i_Index >= 0) && (i_Index < ((int) sm_Objects.size()))), "index (%d) out of range (%d)", i_Index, ((int)sm_Objects.size()) );
		return sm_Objects[i_Index];
	};

	//--------------------------------------------------------------------
	//  Get the pick object pointer by index
	//--------------------------------------------------------------------
	static xxxPickObject* GetPickObject(int i_Index)
	{
		DBG_ASSERT2( (i_Index >= 0 && i_Index < (int) sm_Objects.size()), "index (%d) out of range (%d)", i_Index, (int)sm_Objects.size() );
		return sm_Objects[i_Index]->GetPickObject();
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
	static std::vector<xxxScriptObject*> sm_Objects;
	static bool sm_bShowIcons;

		
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void create_and_add_object(const xxxScriptData &i_Data)
	{
		xxxScriptObject *pObject = xxxObjectCreator::Create(i_Data);
		sm_Objects.push_back(pObject);
	}

};	// end of static class

//============================================================================
// cmmObjectMgrTemplateBase expands the template by adding functions
//	related to picking and visibility of geometry and icons.
//============================================================================
template<class xxxScriptObject, 
		 class xxxPickObject, 
		 class xxxListData, 
		 class xxxScriptData, 
		 class xxxBaseData,
		 class xxxObjectCreator>
class cmmObjectMgrTemplateBase:
	public cmmObjectMgrTemplateNoIcon<xxxScriptObject,xxxPickObject,xxxListData,xxxScriptData,xxxBaseData,xxxObjectCreator>
{
public:

	//--------------------------------------------------------------------
	// Do ray pick on objects
	//--------------------------------------------------------------------
	static bool ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList)
	{
		bool found = false;
		const int num_objects = sm_Objects.size();
		for (int i=0; i<num_objects; i++)
		{
			float tval = 0.0f;
			if ( sm_Objects[i]->RayPick( i_Ray.GetRayStart(), i_Ray.GetRayEnd(), tval ) )
			{
				io_PickList.AddItem(sm_Objects[i]->GetPickObject(), tval);
				found = true;
			}
		}
		return found;
	}

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	static pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode)
	{
		const int num_objects = sm_Objects.size();
		for (int i=0; i<num_objects; i++)
		{
			if ( sm_Objects[i]->GetPickObject()->MatchPickCode( i_PickCode ) )
			{
				return sm_Objects[i]->GetPickObject();
			}
		}
		return NULL;
	}

	//--------------------------------------------------------------------
	//  Changes visible state of object with given index
	//--------------------------------------------------------------------
	static void  SetEditorVisible(int i_Index, bool i_bVisible)
	{
		sm_Objects[i_Index]->SetEditorVisible(i_bVisible);
	}

	//--------------------------------------------------------------------
	//	ShowIcons - show or hide icons that are not part of real scene.
	//--------------------------------------------------------------------
	static void ShowIcons( bool i_bVisible )
	{
		sm_bShowIcons = i_bVisible;
		for (int i=0; i<sm_Objects.size(); i++)
		{
			sm_Objects[i]->ShowIcons(i_bVisible);
		}
	}

	//--------------------------------------------------------------------
	//	are the icons visible?
	//--------------------------------------------------------------------
	static bool IconsVisible()
	{
		return sm_bShowIcons;
	}

};	// end of static class

//============================================================================
// cmmObjectMgrTemplate is only really split off in order to allow
//	certain systems to write threaded loading functions when 
//	doing SetData()
//============================================================================
template<class xxxScriptObject, 
		 class xxxPickObject, 
		 class xxxListData, 
		 class xxxScriptData, 
		 class xxxBaseData,
		 class xxxObjectCreator>
class cmmObjectMgrTemplate :
	public cmmObjectMgrTemplateBase<xxxScriptObject,xxxPickObject,xxxListData,xxxScriptData,xxxBaseData,xxxObjectCreator>
{
public:
	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const xxxListData &i_Data)
	{
		// call the unthreaded variation of the function
		// in the default case. Geometry systems can then
		// use a threaded variation of this function.
		SetDataUnthreaded(i_Data);
	}
};

//============================================================================
// cmmObjectMgrGeomTemplate expands the template by adding functions
//	related to geometry.
//============================================================================
template<class xxxScriptObject, 
		 class xxxPickObject, 
		 class xxxListData, 
		 class xxxScriptData, 
		 class xxxBaseData,
		 class xxxObjectCreator>
class cmmObjectMgrGeomTemplate : 
	public cmmObjectMgrTemplate<xxxScriptObject,xxxPickObject,xxxListData,xxxScriptData,xxxBaseData,xxxObjectCreator>
{
public:
	//--------------------------------------------------------------------
	// Make sure that all geometry is visible for rendering
	//--------------------------------------------------------------------
	static void ConfirmGeometryVisible()
	{
		for (int i=0; i<sm_Objects.size(); i++)
		{
			sm_Objects[i]->SetEditorVisible(true);
		}
	}

};	// end of static class



//============================================================================
// Initialiazing static members
//============================================================================
template<class xxxScriptObject, class xxxPickObject, class xxxListData, class xxxScriptData, class xxxBaseData,	 class xxxObjectCreator>
std::vector<xxxScriptObject*> cmmObjectMgrTemplateBase<xxxScriptObject,xxxPickObject,xxxListData,xxxScriptData,xxxBaseData,xxxObjectCreator>::sm_Objects;

template<class xxxScriptObject, class xxxPickObject, class xxxListData, class xxxScriptData, class xxxBaseData,	 class xxxObjectCreator>
bool cmmObjectMgrTemplateBase<xxxScriptObject,xxxPickObject,xxxListData,xxxScriptData,xxxBaseData,xxxObjectCreator>::sm_bShowIcons = true;
