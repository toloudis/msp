/*****************************************************************************
**	dcutObjectMgr.hpp
**
**	Manages the 3d representation of the cameras in the editor system.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef DCUT_OBJECTMGR_HPP
#error dcutObjectMgr.hpp multiply included
#endif
#define DCUT_OBJECTMGR_HPP

#ifndef DCUT_SCRIPTDATA_HPP
#include "Systems/DirectorsCut/Data/dcutScriptData.hpp"
#endif

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif


//============================================================================
//============================================================================
class dcutDirectorsCutObject;
class dcutScriptObject;
class geoPickRay;
class pick3dPickList;


//============================================================================
//============================================================================
class dcutObjectMgr
{
public:
	//--------------------------------------------------------------------
	//  Clean up
	//--------------------------------------------------------------------
	static void  Clear();

	//--------------------------------------------------------------------
	// Update individual base data
	//--------------------------------------------------------------------
	static void SetBaseData(int i_Index, const dcutCueData& i_Data);
	static dcutCueData GetBaseData(int i_Index);

	//--------------------------------------------------------------------
	// Update individual camera data
	//--------------------------------------------------------------------
	static void SetScriptData(int i_Index, const dcutScriptData& i_Data);
	static dcutScriptData GetScriptData(int i_Index);

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	static dcutCuesData GetData();

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	static void SetData(const dcutCuesData &i_Data);

	//--------------------------------------------------------------------
	// Append objects from this data into list, not allowing duplicates
	//--------------------------------------------------------------------
	static void MergeData(const dcutCuesData &i_Data);

	//--------------------------------------------------------------------
	//  Get number of cameras
	//--------------------------------------------------------------------
	static int GetNumObjects();

	//--------------------------------------------------------------------
	//  Add new camera to world
	//--------------------------------------------------------------------
	static int  AddObject(const dcutScriptData& i_Data);

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
	static int GetIndexForObject(dcutDirectorsCutObject* i_pObject);

	//--------------------------------------------------------------------
	// return index of given Name, returns -1 if not found
	//--------------------------------------------------------------------
	static int GetIndexForObject( const nameString& i_Name );

	//--------------------------------------------------------------------
	//  Get the camera
	//--------------------------------------------------------------------
	static dcutScriptObject* GetObject(int i_Index);
	static dcutDirectorsCutObject* GetPickObject(int i_Index);
	static dcutScriptObject* GetObject( const nameString& i_Name );

	//--------------------------------------------------------------------
	//	build the time in/out lists for all the cameras
	//--------------------------------------------------------------------
	static void BuildTimeInOutLists();

};	// end of static class

