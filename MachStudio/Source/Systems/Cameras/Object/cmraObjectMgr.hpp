/*****************************************************************************
**	cmraObjectMgr.hpp
**
**	Manages the 3d representation of the cameras in the editor system.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_OBJECTMGR_HPP
#error cmraObjectMgr.hpp multiply included
#endif
#define CMRA_OBJECTMGR_HPP

#ifndef CMRA_SCRIPTDATA_HPP
#include "Systems/Cameras/Data/cmraScriptData.hpp"
#endif

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif


//============================================================================
//============================================================================
class cmraCameraObject;
class cmraScriptObject;
class geoPickRay;
class pick3dPickList;
class sel3dObject;
class pick3dPickObject;
class fsResourceTrackerData;


//============================================================================
//============================================================================
class cmraObjectMgr
{
public:
	//--------------------------------------------------------------------
	//  Init and Clean up
	//--------------------------------------------------------------------
	static void  Init();
	static void  CleanUp();

	//--------------------------------------------------------------------
	//  Clean up
	//--------------------------------------------------------------------
	static void  Clear();

	//--------------------------------------------------------------------
	// Update individual base data
	//--------------------------------------------------------------------
	static void SetBaseData(int i_Index, const cmraCameraData& i_Data);
	static cmraCameraData GetBaseData(int i_Index);

	//--------------------------------------------------------------------
	// Update individual camera data
	//--------------------------------------------------------------------
	static void SetScriptData(int i_Index, const cmraScriptData& i_Data);
	static cmraScriptData GetScriptData(int i_Index);

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	static cmraCamerasData GetData();

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const cmraCamerasData &i_Data);

	//--------------------------------------------------------------------
	//  Clear the UIDs of name items belonging to the data object
	//--------------------------------------------------------------------
	static void ClearItemNameUIDs(cmraCamerasData &o_Data);

	//--------------------------------------------------------------------
	// Append objects from this data into list, not allowing duplicates
	//
	//	If i_bRenameDupes is true, the duplicate entry will be renamed
	//	and merged.  If it is false, the duplicate will NOT be merged.
	//--------------------------------------------------------------------
	static void MergeData(const cmraCamerasData &i_Data, bool i_bRenameDupes = false);

	//--------------------------------------------------------------------
	// Select the prtyObject for the editor camera
	//--------------------------------------------------------------------
	static void SelectEditorCameraObject(bool i_bAppend);
	static void DeselectEditorCameraObject();

	//--------------------------------------------------------------------
	// Return pointer to editor camera prtyObject
	//--------------------------------------------------------------------
	static cmraCameraObject* GetEditorCameraObject();

	//--------------------------------------------------------------------
	// Return string to use for editor camera prtyObject
	//--------------------------------------------------------------------
	static std::string GetEditorCameraObjectName();

	//--------------------------------------------------------------------
	//  Get number of cameras
	//--------------------------------------------------------------------
	static int GetNumObjects();

	//--------------------------------------------------------------------
	//  Add new camera to world
	//--------------------------------------------------------------------
	static int  AddObject(const cmraScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Select camera with given index
	//--------------------------------------------------------------------
	static void  SelectObject(int i_Index, bool i_bAppend = false);

	//--------------------------------------------------------------------
	//  Remove camera with given index from selection 
	//--------------------------------------------------------------------
	static void  DeselectObject(int i_Index);

	//--------------------------------------------------------------------
	//  Delete camera with given index
	//--------------------------------------------------------------------
	static void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//	Remap internal name attachments using the given map.
	//  This is part of the duplication process and makes sures 
	//	internal attachments are passed onto the duplicated objects.
	//--------------------------------------------------------------------
	static void RemapNames(int i_Index, 
						   const std::map<nameString, nameString> &i_DuplicateNameMap);

	//--------------------------------------------------------------------
	// return index of given camera, returns -1 if not found
	//--------------------------------------------------------------------
	static int GetIndexForObject(cmraCameraObject* i_pObject);

	//--------------------------------------------------------------------
	// return index of given Name, returns -1 if not found
	//--------------------------------------------------------------------
	static int GetIndexForObject( const nameString& i_Name );

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	static pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode);

	//--------------------------------------------------------------------
	//  Get the camera
	//--------------------------------------------------------------------
	static cmraScriptObject* GetObject(int i_Index);
	static cmraCameraObject* GetPickObject(int i_Index);
	static cmraScriptObject* GetObject( const nameString& i_Name );

	//--------------------------------------------------------------------
	//	ShowIcons - show or hide icons that are not part of real scene.
	//--------------------------------------------------------------------
	static void ShowIcons( bool i_bVisible );

	//--------------------------------------------------------------------
	//	are the icons visible?
	//--------------------------------------------------------------------
	static bool IconsVisible();

	//--------------------------------------------------------------------
	// Set object active state in the render layer
	//--------------------------------------------------------------------
	static void SetActiveRenderLayer(int i_Index, bool i_bActive);

	//--------------------------------------------------------------------
	//  Changes visible state of object with given index
	//--------------------------------------------------------------------
	static void  SetEditorVisible(int i_Index, bool i_bVisible);

	//--------------------------------------------------------------------
	//  Returns the visible state of object with given index
	//--------------------------------------------------------------------
	static bool GetEditorVisible(int i_Index);

	//--------------------------------------------------------------------
	//	Generate a new camera name
	//--------------------------------------------------------------------
	static void GenerateNewCameraName(nameString& i_Name, 
									  nameString& o_Name, 
									  bool i_bOrthographic);

	//--------------------------------------------------------------------
	//	build the time in/out lists for all the cameras
	//--------------------------------------------------------------------
	static void BuildTimeInOutLists();

	//--------------------------------------------------------------------
	//  get a list of resources.  the resources will be appended to the
	//	passed in list.
	//--------------------------------------------------------------------
	static void GetResourceList( fsResourceTrackerData& io_List );

};	// end of static class

