/*****************************************************************************
**	cmmSingleObjectMgrTemplate.hpp
**
**	Template for object managers that maintain a single script object
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SINGLEOBJECTMGRTEMPLATE_HPP
#error cmmSingleObjectMgrTemplate.hpp multiply included
#endif
#define CMM_SINGLEOBJECTMGRTEMPLATE_HPP

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
class pick3dPickObject;


//============================================================================
// cmmObjectMgrTemplateNoIcon defines the implementation of an object
//	manager for a system that contains an object that is split such that a 
//	script object contains a property object.
//============================================================================
template<class xxxScriptObject, 
		 class xxxPickObject, 
		 class xxxScriptData, 
		 class xxxBaseData>
class cmmSingleObjectMgrTemplateNoIcon
{
public:
	//--------------------------------------------------------------------
	// Update individual base data
	//--------------------------------------------------------------------
	static void SetBaseData(const xxxBaseData& i_Data)
	{
		sm_Object->SetBaseData(i_Data);
	}
	static xxxBaseData GetBaseData()
	{
		return sm_Object->GetBaseData();
	}

	//--------------------------------------------------------------------
	// Update individual object data
	//--------------------------------------------------------------------
	static void SetScriptData(const xxxScriptData& i_Data)
	{
		sm_Object->SetScriptData(i_Data);
	}
	static xxxScriptData GetScriptData()
	{
		return sm_Object->GetScriptData();
	}

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	static xxxScriptData GetData()
	{
		return sm_Object->GetScriptData();
	}

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetDataUnthreaded(const xxxScriptData &i_Data)
	{
		sm_Object->SetScriptData(i_Data);
	}

	//--------------------------------------------------------------------
	// Append objects from this data into list, not allowing duplicates
	//
	//	If i_bRenameDupes is true, the duplicate entry will be renamed
	//	and merged.  If it is false, the duplicate will NOT be merged.
	//--------------------------------------------------------------------
	static void MergeData(const xxxScriptData &i_Data, bool i_bRenameDupes = false)
	{
		sm_Object->SetScriptData(i_Data);
	}

	//--------------------------------------------------------------------
	//  Add new object to world
	//--------------------------------------------------------------------
	static int AddObject(const xxxScriptData& i_Data)
	{
		// either replace, or don't!
		try
		{
			sm_Object = new xxxScriptObject();
			sm_Object->SetScriptData(i_Data);
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

			DBG_ERROR( msg );
			guiMessageBox::Show(msg.c_str(),"Error Loading Object", guiMessageBox::e_OKOnly);

			sm_Object = NULL;//?
			return -1;
			//assert(false);
		}

		// return the index of the object
		return 0;
	}

	//--------------------------------------------------------------------
	//  Select object with given index
	//--------------------------------------------------------------------
	static void SelectObject(bool i_bAppend = false)
	{
		if (sel3dMgr::GetSelected() != sm_Object->GetPickObject())
		{
			if (i_bAppend)
				sel3dMgr::AddToSelection(sm_Object->GetPickObject());
			else
				sel3dMgr::Select(sm_Object->GetPickObject());
		}
	}

	//--------------------------------------------------------------------
	//  Remove object with given index from selection
	//--------------------------------------------------------------------
	static void  DeselectObject()
	{
		//if (sel3dMgr::GetSelected() != sm_Object.GetPickObject())
		{
			sel3dMgr::RemoveFromSelection(sm_Object->GetPickObject());
		}
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	static void  DeleteObject()
	{
		// Note: could use a function here in xxxObjectCreator in order to 
		// do anything before deletion.
		sel3dMgr::ClearSelection();

		// clear the data to default?
		delete sm_Object;
		sm_Object = NULL;
//		sm_Object->SetScriptData(xxxScriptData());
	}

	//--------------------------------------------------------------------
	// return index of given object, returns -1 if not found
	//--------------------------------------------------------------------
	static int GetIndexForObject(xxxPickObject* i_pObject)
	{
		if (sm_Object->GetPickObject() == i_pObject)
		{
			return 0;
		}
		return -1;
	}

	//--------------------------------------------------------------------
	//  Return number of objects
	//--------------------------------------------------------------------
	static int GetNumObjects()
	{
		return (sm_Object == NULL) ? 0 : 1;
	}

	//--------------------------------------------------------------------
	//  Get the script object pointer
	//--------------------------------------------------------------------
//	static xxxScriptObject* GetObject()
//	{
//		return &sm_Object;
//	}

	//--------------------------------------------------------------------
	//  Get the pick object pointer by index
	//--------------------------------------------------------------------
	static xxxPickObject* GetPickObject()
	{
		return sm_Object->GetPickObject();
	}

	//--------------------------------------------------------------------
	//  clear
	//--------------------------------------------------------------------
	static void Clear()
	{
		sel3dMgr::ClearSelection();
		sm_Object->SetScriptData(xxxScriptData());
	}

	//--------------------------------------------------------------------
	//  get a list of resources.  the resources will be appended to the
	//	passed in list.
	//--------------------------------------------------------------------
	static void GetResourceList( fsResourceTrackerData& io_List )
	{
		sm_Object->GetResourceList( io_List );
	}

	//--------------------------------------------------------------------
	//	return true if the filename is NOT a dupe
	//--------------------------------------------------------------------
//	static bool  VerifyNodupeName( char* i_Name )
//	{
//		if ( sm_Object.GetName().GetString() == i_Name )
//		{
//			return false;
//		}
//		return true;
//	}

protected:
	static xxxScriptObject* sm_Object;
	static bool sm_bShowIcons;

		
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
//	static void create_and_add_object(const xxxScriptData &i_Data)
//	{
//		xxxScriptObject *pObject = xxxObjectCreator::Create(i_Data);
//		sm_Object = pObject;
//	}

};	// end of static class



//============================================================================
// cmmObjectMgrTemplateBase expands the template by adding functions
//	related to picking and visibility of geometry and icons.
//============================================================================
template<class xxxScriptObject, 
		 class xxxPickObject, 
		 class xxxScriptData, 
		 class xxxBaseData>
class cmmSingleObjectMgrTemplateBase:
	public cmmSingleObjectMgrTemplateNoIcon<xxxScriptObject,xxxPickObject,xxxScriptData,xxxBaseData>
{
public:

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	static pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode)
	{
		if ( sm_Object->GetPickObject()->MatchPickCode( i_PickCode ) )
		{
			return sm_Object->GetPickObject();
		}
		return NULL;
	}

	//--------------------------------------------------------------------
	// Set object active state in the render layer
	//--------------------------------------------------------------------
	static void SetActiveRenderLayer(bool i_bActive)
	{
		sm_Object->SetActiveRenderLayer(i_bActive);
	}

	//--------------------------------------------------------------------
	//  Changes visible state of object with given index
	//--------------------------------------------------------------------
	static void  SetEditorVisible(bool i_bVisible)
	{
		sm_Object->SetEditorVisible(i_bVisible);
	}

	//--------------------------------------------------------------------
	//  Returns the visible state of object with given index
	//--------------------------------------------------------------------
	static bool  GetEditorVisible()
	{
		return sm_Object->GetEditorVisible();
	}

	//--------------------------------------------------------------------
	//	ShowIcons - show or hide icons that are not part of real scene.
	//--------------------------------------------------------------------
	static void ShowIcons( bool i_bVisible )
	{
		sm_bShowIcons = i_bVisible;
		sm_Object->ShowIcons(i_bVisible);
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
		 class xxxScriptData, 
		 class xxxBaseData>
class cmmSingleObjectMgrTemplate :
	public cmmSingleObjectMgrTemplateBase<xxxScriptObject,xxxPickObject,xxxScriptData,xxxBaseData>
{
public:
	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const xxxScriptData &i_Data)
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
		 class xxxScriptData, 
		 class xxxBaseData>
class cmmSingleObjectMgrGeomTemplate : 
	public cmmSingleObjectMgrTemplate<xxxScriptObject,xxxPickObject,xxxScriptData,xxxBaseData>
{
public:
	//--------------------------------------------------------------------
	// Make sure that all geometry is visible for rendering
	//--------------------------------------------------------------------
	static void ConfirmGeometryVisible()
	{
		sm_Object->SetEditorVisible(true);
	}

};	// end of static class



//============================================================================
// Initialiazing static members
//============================================================================
template<class xxxScriptObject, class xxxPickObject, class xxxScriptData, class xxxBaseData>
xxxScriptObject* cmmSingleObjectMgrTemplateBase<xxxScriptObject,xxxPickObject,xxxScriptData,xxxBaseData>::sm_Object = NULL;

template<class xxxScriptObject, class xxxPickObject, class xxxScriptData, class xxxBaseData>
bool cmmSingleObjectMgrTemplateBase<xxxScriptObject,xxxPickObject,xxxScriptData,xxxBaseData>::sm_bShowIcons = true;
