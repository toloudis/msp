/*****************************************************************************
**	cmraObjectMgr.hpp
**
**	Manages the 3d representation of the cameras in the editor system.
**
**	Extra Large Technology
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
class pick3dPickObject;


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
	// Append objects from this data into list, not allowing duplicates
	//--------------------------------------------------------------------
	static void MergeData(const cmraCamerasData &i_Data);

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
	// return index of given camera, returns -1 if not found
	//--------------------------------------------------------------------
	static int GetIndexForObject(cmraCameraObject* i_pObject);

	//--------------------------------------------------------------------
	// return index of given Name, returns -1 if not found
	//--------------------------------------------------------------------
	static int GetIndexForObject( const nameString& i_Name );

	//--------------------------------------------------------------------
	// Do ray pick on camera objects
	//--------------------------------------------------------------------
	static bool ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList);

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
	//  Changes visible state of object with given index
	//--------------------------------------------------------------------
	static void  SetEditorVisible(int i_Index, bool i_bVisible);

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

};	// end of static class

