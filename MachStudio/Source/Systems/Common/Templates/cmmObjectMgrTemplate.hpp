/*****************************************************************************
**	cmmObjectMgrTemplate.hpp
**
**	Template for object managers that maintain a list of script objects
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_OBJECTMGRTEMPLATE_HPP
#error cmmObjectMgrTemplate.hpp multiply included
#endif
#define CMM_OBJECTMGRTEMPLATE_HPP

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
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
#ifndef NAME_MGR_HPP
#include "Core/name/nameMgr.hpp"
#endif
#ifndef SEL3D_MGR_HPP
#include "Tool/sel3d/sel3dMgr.hpp"
#endif

#ifndef GRAPHICS_LAYER_HPP
#include "Graphics/GraphicsLayer.hpp"
#endif
#ifndef G2D_SYSTEM_HPP
#include "Graphics/g2d/g2dSystem.hpp"
#endif

#include <map>
#include <vector>
#include <assert.h>
#include <sstream>


//============================================================================
//	forward references
//============================================================================
class geoPickRay;
class pick3dPickList;
class pick3dPickObject;


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
	//
	//	If i_bRenameDupes is true, the duplicate entry will be renamed
	//	and merged.  If it is false, the duplicate will NOT be merged.
	//--------------------------------------------------------------------
	static void MergeData(const xxxListData &i_Data, bool i_bRenameDupes = false)
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
					if (!i_bRenameDupes)
					{
						// found a duplicate name, don't add this object
						std::ostringstream oss;
						oss<<i_Data.m_Items[i].m_BaseData.m_Name.GetString()<<" exists in the scene.  ";
						oss<<"File will not be imported";
						std::string msg(oss.str());
						guiMessageBox::Show(msg.c_str(),"Error Loading Object", guiMessageBox::e_OKOnly);
					}
					bDuplicate = true;
					break;
				}
			}

			//	depending on flags add the object, rename it, or nothing
			if (!bDuplicate)
			{
				create_and_add_object( i_Data.m_Items[i] );
			}
			else if (bDuplicate && i_bRenameDupes)
			{
				//	pick a new name and add
				create_and_add_object( i_Data.m_Items[i] );
			}
		}
	}

	//--------------------------------------------------------------------
	//  Add new object to world
	//--------------------------------------------------------------------
	static int AddObject(const xxxScriptData& i_Data) 
	{
		bool isCreateObjectSuccess = true;
		try
		{
			isCreateObjectSuccess = create_and_add_object( i_Data );
		}
		catch( fsFileDoesntExistX& i_Ex )
		{
			//char msg[512];
			std::string txt;
			txt = itStringUtil::GetStdString(i_Ex.GetLocator().GetLastName());
			//sprintf( msg, "Cannot add the object -- missing %s", txt.c_str() );
			std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss << "Cannot add the object -- missing "<<txt;
			std::string msg(oss.str());
			DBG_ERROR( msg.c_str() );
			guiMessageBox::Show(msg.c_str(),"Error Loading Object", guiMessageBox::e_OKOnly);

			return -1;
			//assert(false);
		}

		// Some errors occurred during create and add object
		if (!isCreateObjectSuccess)
			return -1;

		int index = sm_Objects.size() - 1;
//		SelectObject(index);
		return index;
	}

	//--------------------------------------------------------------------
	//  Select object with given index
	//--------------------------------------------------------------------
	static void SelectObject(int i_Index, bool i_bAppend = false)
	{
		if (sel3dMgr::GetSelected() != sm_Objects[i_Index]->GetPickObject())
		{
			if (i_bAppend)
				sel3dMgr::AddToSelection(sm_Objects[i_Index]->GetPickObject());
			else
				sel3dMgr::Select(sm_Objects[i_Index]->GetPickObject());
		}
	}

	//--------------------------------------------------------------------
	//  Remove object with given index from selection
	//--------------------------------------------------------------------
	static void DeselectObject(int i_Index)
	{
		//if (sel3dMgr::GetSelected() != sm_Objects[i_Index]->GetPickObject())
		{
			sel3dMgr::RemoveFromSelection(sm_Objects[i_Index]->GetPickObject());
		}
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	static void DeleteObject(int i_Index)
	{
		// Note: could use a function here in xxxObjectCreator in order to 
		// do anything before deletion.
		sel3dMgr::ClearSelection();

		delete sm_Objects[i_Index];
		sm_Objects.erase(sm_Objects.begin() + i_Index);
	}

	
	//--------------------------------------------------------------------
	//	Remap internal name attachments using the given map.
	//  This is part of the duplication process and makes sures 
	//	internal attachments are passed onto the duplicated objects.
	//--------------------------------------------------------------------
	static void RemapNames(int i_Index, 
						   const std::map<nameString, nameString> &i_DuplicateNameMap)
	{
		sm_Objects[i_Index]->RemapNames(i_DuplicateNameMap);
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
	}

	//--------------------------------------------------------------------
	//  Get the script object pointer by index
	//--------------------------------------------------------------------
	static xxxScriptObject* GetObject(int i_Index)
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < (int) sm_Objects.size()), "index out of range " << i_Index << " < " <<  (int)sm_Objects.size() );
		return sm_Objects[i_Index];
	}

	//--------------------------------------------------------------------
	//  Get the pick object pointer by index
	//--------------------------------------------------------------------
	static xxxPickObject* GetPickObject(int i_Index)
	{
		DBG_ASSERT( (i_Index >= 0 && i_Index < (int) sm_Objects.size()), "index out of range " << i_Index << " < " <<  (int)sm_Objects.size() );
		return sm_Objects[i_Index]->GetPickObject();
	}

	//--------------------------------------------------------------------
	//  clear
	//--------------------------------------------------------------------
	static void Clear()
	{
		sel3dMgr::ClearSelection();
		envSTLHelpers::DeleteContainer(sm_Objects);
		if (GraphicsLayer::GetSystem2D() != NULL)
			GraphicsLayer::GetSystem2D()->ClearManagedResources();
	}

	//--------------------------------------------------------------------
	//  get a list of resources.  The resources will be appended to the
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
	static bool VerifyNodupeName( char* i_Name )
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

	//--------------------------------------------------------------------
	//	create a default name for the new object
	//--------------------------------------------------------------------
	static void create_default_name( const itString& i_Filename, 
									nameString& o_NameString, 
									int& io_ObjectCounter)
	{
		nameMgr::VerifyNoDupNameFunc verifyFunc = &VerifyNodupeName;
		nameMgr::CreateDefaultName( 
			nameString(itStringUtil::GetStdString( i_Filename ).c_str()),
			verifyFunc,
			o_NameString,
			io_ObjectCounter
			);
	}

	//--------------------------------------------------------------------
	//	create a new name according to the original name and increase
	//	the ending number
	//--------------------------------------------------------------------
	static void create_duplicate_name( const itString& i_Filename, 
										nameString& o_NameString)
	{
		nameMgr::VerifyNoDupNameFunc verifyFunc = &VerifyNodupeName;
		nameMgr::CreateDuplicateName(
			nameString(itStringUtil::GetStdString( i_Filename ).c_str()),
			verifyFunc,
			o_NameString);
	}

protected:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static bool create_and_add_object(const xxxScriptData &i_Data)
	{
		xxxScriptObject *pObject = xxxObjectCreator::Create(i_Data);
		if (pObject)
		{
			sm_Objects.push_back(pObject);
			return true;
		}
		return false;
	}

protected:
	static std::vector<xxxScriptObject*> sm_Objects;
	static bool sm_bShowIcons;

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
	// Set object active state in the render layer
	//--------------------------------------------------------------------
	static void SetActiveRenderLayer(int i_Index, bool i_bActive)
	{
		sm_Objects[i_Index]->SetActiveRenderLayer(i_bActive);
	}

	//--------------------------------------------------------------------
	//  Changes visible state of object with given index
	//--------------------------------------------------------------------
	static void SetEditorVisible(int i_Index, bool i_bVisible)
	{
		sm_Objects[i_Index]->SetEditorVisible(i_bVisible);
	}

	//--------------------------------------------------------------------
	//  Returns the visible state of object with given index
	//--------------------------------------------------------------------
	static bool  GetEditorVisible(int i_Index)
	{
		return sm_Objects[i_Index]->GetEditorVisible();
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

	//--------------------------------------------------------------------
	//  Clear the UIDs of name items belonging to the data object
	//--------------------------------------------------------------------
	static void ClearItemNameUIDs(xxxListData &o_Data);
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

