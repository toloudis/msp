/*****************************************************************************
**	cmraObjectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/tmln/tmlnTimeInOutMgr.hpp"
#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"
#include "Systems/Cameras/Timeline/cmraChannelCapture.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoPickRay.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/name/nameMgr.hpp"
#include "Tool/pick3d/pick3dPickList.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace
{
	std::vector<cmraScriptObject*> l_Cameras;
	int l_ObjectCounter = 0;
	bool l_bIconsVisible = true;

	//--------------------------------------------------------------------
	// Global prtyObject for setting editor camera properties
	shared_ptr<cmraCameraObject> l_EditorCameraObject;

	//--------------------------------------------------------------------
	void clear_cameras()
	{
		envSTLHelpers::DeleteContainer(l_Cameras);
	}

	//--------------------------------------------------------------------
	//	return true if the filename is NOT a dupe
	bool verify_nodupe_name( char* i_Name )
	{
		for (int i=0; i<l_Cameras.size(); i++)
		{
			if ( l_Cameras[i]->GetName().GetString() == i_Name )
			{
				return false;
			}
		}
		return true;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void create_default_name( const itString& i_Filename, nameString& o_NameString )
	{	
		nameMgr::VerifyNoDupNameFunc verifyFunc = &verify_nodupe_name;
		nameMgr::CreateDefaultName( 
			nameString(itStringUtil::GetStdString( i_Filename ).c_str()),
			verifyFunc,
			o_NameString,
			l_ObjectCounter
			);
		//DBG_LOG( "Added -- name (" << strName << ")"  );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void create_duplicate_name( const itString& i_Filename, nameString& o_NameString )
	{
		nameMgr::VerifyNoDupNameFunc verifyFunc = &verify_nodupe_name;
		nameMgr::CreateDuplicateName(
			nameString(itStringUtil::GetStdString( i_Filename ).c_str()),
			verifyFunc,
			o_NameString);
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void generate_camera_name(nameString& i_Name, nameString& o_Name, bool i_bOrthographic)
	{
		//	if there is a name in the data, use that instead of
		//	creating a generic name.
		if (i_Name.IsEmpty())
		{
			itString base((i_bOrthographic) ? "ORTHO" : "CAM" );
			create_default_name( base, o_Name );
		}
		else
		{
			// use the original name as based to generate the new name
			itString base(i_Name.GetString().c_str());
			create_duplicate_name(base, o_Name);
			//o_Name = i_Name;
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void create_camera(const cmraScriptData &i_Data)
	{
		//	if there isn't a name, create one
		//nameString baseName = i_Data.m_BaseData.m_Name.GetValue();
		//nameString objName;
		//generate_camera_name(baseName, objName, i_Data.m_BaseData.m_bOrthographic.GetValue() );

		// create object for camera
		cmraScriptObject *camera = new cmraScriptObject(i_Data, 
			i_Data.m_BaseData.m_Name.GetValue());

		// Maintain the current icon visibility
		camera->ShowIcons(l_bIconsVisible);

		//	add the camera to the mgrs
		l_Cameras.push_back(camera);

		//DBG_LOG2("Adding camera: %s - %s", camera->GetName().GetString().c_str(), i_Data.m_BaseData.m_Description.GetValue().c_str());
		camsCameraMgr::AddCamera( camera->GetName(), 
				i_Data.m_BaseData.m_Description.GetValue(),
				camera->GetCameraPtr(),
				camera->GetCameraProxyPtr());
	}

}	// end of namespace


//--------------------------------------------------------------------
//  Init and Clean up
//--------------------------------------------------------------------
void  cmraObjectMgr::Init()
{
	// Create prtyObject for editor camera
	const bool c_EditorCameraOrthographic = false;
	const bool c_ControlEditorCamera = true;
	l_EditorCameraObject.reset(new cmraCameraObject(c_EditorCameraOrthographic, c_ControlEditorCamera));
}
void  cmraObjectMgr::CleanUp()
{
	// Clean up prtyObject for editor camera
	l_EditorCameraObject.reset();
}

//--------------------------------------------------------------------
//  Clear
//--------------------------------------------------------------------
void  cmraObjectMgr::Clear()
{
	camsFollowUtil::SetEditorCamera();
	camsCameraMgr::Clear();
	clear_cameras();
}

//--------------------------------------------------------------------
// Update individual base data
//--------------------------------------------------------------------
void cmraObjectMgr::SetBaseData(int i_Index, const cmraCameraData& i_Data)
{
	l_Cameras[i_Index]->SetBaseData(i_Data);
}
cmraCameraData cmraObjectMgr::GetBaseData(int i_Index)
{
	return l_Cameras[i_Index]->GetBaseData();
}

//--------------------------------------------------------------------
// Update individual camera data
//--------------------------------------------------------------------
void cmraObjectMgr::SetScriptData(int i_Index, const cmraScriptData& i_Data)
{
	DBG_ASSERT( (i_Index >= 0 && i_Index < l_Cameras.size()), "index out of range" );

	l_Cameras[i_Index]->SetScriptData(i_Data);
	camsCameraMgr::SetCameraName(i_Index, i_Data.m_BaseData.m_Name.GetValue());
	camsCameraMgr::SetCameraDescription(i_Index, i_Data.m_BaseData.m_Description.GetValue());
	
	// notify_callbacks
}
cmraScriptData cmraObjectMgr::GetScriptData(int i_Index)
{
	return l_Cameras[i_Index]->GetScriptData();
}

//--------------------------------------------------------------------
//  Access to whole data as one structure for easy display and
// parsing
//--------------------------------------------------------------------
cmraCamerasData cmraObjectMgr::GetData()
{
	cmraCamerasData data;
	for (int i=0; i<l_Cameras.size(); i++)
	{
		data.m_Items.push_back( l_Cameras[i]->GetScriptData() );
	}

	// Get editor camera data
	data.m_EditorCamera = l_EditorCameraObject->GetData();

	return data;
}

//--------------------------------------------------------------------
// Set data as a whole
//--------------------------------------------------------------------
void cmraObjectMgr::SetData(const cmraCamerasData &i_Data)
{
	camsFollowUtil::SetEditorCamera();
	clear_cameras();
	for (int i=0; i<i_Data.m_Items.size(); i++)
	{
		create_camera(i_Data.m_Items[i]);
	}

	// Set editor camera data
	l_EditorCameraObject->SetData(i_Data.m_EditorCamera);
}

//--------------------------------------------------------------------
//  Clear the UIDs of name items belonging to the data object
//--------------------------------------------------------------------
void cmraObjectMgr::ClearItemNameUIDs(cmraCamerasData &o_Data)
{}

//--------------------------------------------------------------------
// Append objects from this data into list, not allowing duplicates
//
//	If i_bRenameDupes is true, the duplicate entry will be renamed
//	and merged.  If it is false, the duplicate will NOT be merged.
//--------------------------------------------------------------------
//static 
void cmraObjectMgr::MergeData(const cmraCamerasData &i_Data, bool i_bRenameDupes)
{
	const int num_items = i_Data.m_Items.size();
	for (int i=0; i<num_items; i++)
	{
		bool bDuplicate = false;
		const int num_objects = l_Cameras.size();
		for (int j=0; j<num_objects; j++)
		{
			if ( i_Data.m_Items[i].m_BaseData.m_Name.GetString() == 
					l_Cameras[j]->GetName().GetString() )
			{
				// found a duplicate name, don't add this object
				bDuplicate = true;
				break;
			}
		
		}
		if (!bDuplicate)
		{
			create_camera( i_Data.m_Items[i] );
		}
		else if (bDuplicate && i_bRenameDupes)
		{
			//	pick a new name and add
			create_camera( i_Data.m_Items[i] );
		}
	}
}


//--------------------------------------------------------------------
// Select the prtyObject for the editor camera
//--------------------------------------------------------------------
void cmraObjectMgr::SelectEditorCameraObject(bool i_bAppend)
{
	if (sel3dMgr::GetSelected() != l_EditorCameraObject.get())
	{
		if (i_bAppend)
			sel3dMgr::AddToSelection(l_EditorCameraObject.get());
		else
			sel3dMgr::Select(l_EditorCameraObject.get());
	}
}
void cmraObjectMgr::DeselectEditorCameraObject()
{
	//if (sel3dMgr::GetSelected() != l_EditorCameraObject.get())
	{
		sel3dMgr::RemoveFromSelection(l_EditorCameraObject.get());
	}
}

//--------------------------------------------------------------------
// Return pointer to editor camera prtyObject
//--------------------------------------------------------------------
cmraCameraObject* cmraObjectMgr::GetEditorCameraObject()
{
	DBG_ASSERT(l_EditorCameraObject.get(), "Editor camera prtyObject is NULL.");
	return l_EditorCameraObject.get();
}

//--------------------------------------------------------------------
// Return string to use for editor camera prtyObject
//--------------------------------------------------------------------
std::string cmraObjectMgr::GetEditorCameraObjectName()
{
	return "Editor Camera";
}

//--------------------------------------------------------------------
//  Get number of cameras
//--------------------------------------------------------------------
int cmraObjectMgr::GetNumObjects()
{
	return l_Cameras.size();
}

//--------------------------------------------------------------------
//  Add new camera to world
//--------------------------------------------------------------------
int  cmraObjectMgr::AddObject(const cmraScriptData& i_Data)
{
	create_camera(i_Data);

	int index = l_Cameras.size() - 1;
	return index;
}

//--------------------------------------------------------------------
//  Select camera with given index
//--------------------------------------------------------------------
void  cmraObjectMgr::SelectObject(int i_Index, bool i_bAppend)
{
	DBG_ASSERT( (i_Index >= 0 && i_Index < l_Cameras.size()), "index out of range" );

	if (sel3dMgr::GetSelected() != l_Cameras[i_Index]->GetPickObject())
	{
		if (i_bAppend)
		{
			sel3dMgr::AddToSelection(l_Cameras[i_Index]->GetPickObject());
		}
		else
		{
			sel3dMgr::Select(l_Cameras[i_Index]->GetPickObject());
		}
	}
}

//--------------------------------------------------------------------
//  Remove camera with given index from selection 
//--------------------------------------------------------------------
void  cmraObjectMgr::DeselectObject(int i_Index)
{
	DBG_ASSERT( (i_Index >= 0 && i_Index < l_Cameras.size()), "index out of range" );

	if (sel3dMgr::GetSelected() != l_Cameras[i_Index]->GetPickObject())
	{
		sel3dMgr::RemoveFromSelection(l_Cameras[i_Index]->GetPickObject());
	}
}

//--------------------------------------------------------------------
//  Delete camera with given index
//--------------------------------------------------------------------
void  cmraObjectMgr::DeleteObject(int i_Index)
{
	DBG_ASSERT( (i_Index >= 0 && i_Index < l_Cameras.size()), "index out of range" );

	// shift follow index down when deleting camera
	if (camsFollowUtil::GetFollowIndex() > i_Index)
		camsFollowUtil::SetFollowIndex(camsFollowUtil::GetFollowIndex()-1);
	else if (camsFollowUtil::GetFollowIndex() == i_Index)
		camsFollowUtil::SetEditorCamera();

	//DBG_LOG3("deleting camera #%d %s out of %d cameras", i_Index, l_Cameras[i_Index]->GetPickObject()->GetName().GetString().c_str(), l_Cameras.size() );

	sel3dMgr::ClearSelection();

	camsCameraMgr::RemoveCamera(i_Index);
	delete l_Cameras[i_Index];
	l_Cameras.erase(l_Cameras.begin() + i_Index);

	//DBG_LOG(" deleted camera. #" << l_Cameras.size() << " cameras left"  );
}

//--------------------------------------------------------------------
//	Remap internal name attachments using the given map.
//  This is part of the duplication process and makes sures 
//	internal attachments are passed onto the duplicated objects.
//--------------------------------------------------------------------
void cmraObjectMgr::RemapNames(int i_Index, 
							  const std::map<nameString, nameString> &i_DuplicateNameMap)
{
	l_Cameras[i_Index]->RemapNames(i_DuplicateNameMap);
}

//--------------------------------------------------------------------
// return index of given camera, returns -1 if not found
//--------------------------------------------------------------------
int cmraObjectMgr::GetIndexForObject(cmraCameraObject* i_pObject)
{
	for (int i=0; i<l_Cameras.size(); i++)
		if (l_Cameras[i]->GetPickObject() == i_pObject)
			return i;
	return -1;
}

//--------------------------------------------------------------------
// return index of given Name, returns -1 if not found
//--------------------------------------------------------------------
int cmraObjectMgr::GetIndexForObject( const nameString& i_Name )
{
	for (int i=0; i<l_Cameras.size(); i++)
	{
		if (l_Cameras[i]->GetPickObject()->GetName() == i_Name)
		{
			return i;
		}
	}
	return -1;
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
pick3dPickObject* cmraObjectMgr::MatchPickCode(envType::UInt32 i_PickCode)
{
	const int num_objects = l_Cameras.size();
	for (int i=0; i<num_objects; i++)
	{
		if ( l_Cameras[i]->GetPickObject()->MatchPickCode( i_PickCode ) )
		{
			return l_Cameras[i]->GetPickObject();
		}
	}
	return NULL;
}

//--------------------------------------------------------------------
//  Get the camera
//--------------------------------------------------------------------
cmraScriptObject* cmraObjectMgr::GetObject(int i_Index)
{
	DBG_ASSERT( (i_Index >= 0 && i_Index < l_Cameras.size()), "index out of range" );

	return l_Cameras[i_Index];
}
cmraCameraObject* cmraObjectMgr::GetPickObject(int i_Index)
{
	DBG_ASSERT( (i_Index >= 0 && i_Index < l_Cameras.size()), "index out of range" );

	return l_Cameras[i_Index]->GetPickObject();
}
cmraScriptObject* cmraObjectMgr::GetObject( const nameString& i_Name )
{
	int index = cmraObjectMgr::GetIndexForObject( i_Name );

	if ( index != -1 )
		return l_Cameras[ index ];
	else
		return 0;
}

//--------------------------------------------------------------------
//	ShowIcons - show or hide icons that are not part of real scene.
//--------------------------------------------------------------------
void cmraObjectMgr::ShowIcons( bool i_bVisible )
{
	l_bIconsVisible = i_bVisible;

	for (int i=0; i<l_Cameras.size(); i++)
	{
		l_Cameras[i]->ShowIcons(i_bVisible);
	}
}

//--------------------------------------------------------------------
//	are the icons visible?
//--------------------------------------------------------------------
bool cmraObjectMgr::IconsVisible()
{
	return l_bIconsVisible;
}

//--------------------------------------------------------------------
// Set object active state in the render layer
//--------------------------------------------------------------------
//static 
void cmraObjectMgr::SetActiveRenderLayer(int i_Index, bool i_bActive)
{
	l_Cameras[i_Index]->SetActiveRenderLayer(i_bActive);
}

//--------------------------------------------------------------------
//  Changes visible state of object with given index
//--------------------------------------------------------------------
//static 
void  cmraObjectMgr::SetEditorVisible(int i_Index, bool i_bVisible)
{
	l_Cameras[i_Index]->SetEditorVisible(i_bVisible);
}

//--------------------------------------------------------------------
//  Returns the visible state of object with given index
//--------------------------------------------------------------------
//static 
bool  cmraObjectMgr::GetEditorVisible(int i_Index)
{
	return l_Cameras[i_Index]->GetEditorVisible();
}

//--------------------------------------------------------------------
//	send the camera names tp the camsCameraMgr
//--------------------------------------------------------------------
//void cmraObjectMgr::SyncCameraNames()
//{
//	//DBG_LOG("cmraObjectMgr cameras+++++++++++++++++++++++++");
//	camsCameraMgr::Clear();
//	for (int i=0; i<l_Cameras.size(); i++)
//	{
//		cmraCameraObject *pCamera = l_Cameras[i]->GetPickObject();
//		camsCameraMgr::AddCamera( pCamera->GetName(), 
//								  pCamera->GetPropertyDescription().GetValue(),
//								  &pCamera->Camera());
//
//		//nameString name = l_Cameras[i]->GetName();
//		//DBG_LOG2("%d - %s", name.GetUID(), name.GetString().c_str() );
//	}
//}

//--------------------------------------------------------------------
//	Generate a new camera name
//--------------------------------------------------------------------
void cmraObjectMgr::GenerateNewCameraName(nameString& i_Name, nameString& o_Name, bool i_bOrthographic)
{
	generate_camera_name(i_Name, o_Name, i_bOrthographic);
}

//--------------------------------------------------------------------
//	build the time in/out lists for all the cameras
//--------------------------------------------------------------------
void cmraObjectMgr::BuildTimeInOutLists()
{
	//	Get the in/out time for the cameras
	//
	for (int i=0; i<l_Cameras.size(); i++)
	{
		std::string& camname = l_Cameras[i]->GetName().GetString();
		//DBG_LOG("building IN-OUT lists for camera " << camname.c_str() );
		cmraScriptObject* pObject = GetObject(l_Cameras[i]->GetName());
		if (pObject != 0)
		{
			pObject->CaptureChannel().GetInOutTimes(tmlnTimeInOutMgr::GetDataList(camname));
		}

		// DEBUG ONLY
		//
		//tmlnTimeInOutDataList tiolist = tmlnTimeInOutMgr::GetDataList( camname );
		//tiolist.Debug_DisplayData();
	}
}


//--------------------------------------------------------------------
//  get a list of resources.  the resources will be appended to the
//	passed in list.
//--------------------------------------------------------------------
void cmraObjectMgr::GetResourceList( fsResourceTrackerData& io_List )
{
	const int num_objects = l_Cameras.size();
	for (int i=0; i < num_objects; i++)
	{
		l_Cameras[i]->GetResourceList( io_List );
	}
}
