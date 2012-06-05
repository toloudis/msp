/*****************************************************************************
**	dcutDataMgr.hpp
**
**	Adapter object for DocumentChunk template...
**	manages split of dcutCuesData between ObjectMgr and Cue data
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef DCUT_DATAMGR_HPP
#error dcutDataMgr.hpp multiply included
#endif
#define DCUT_DATAMGR_HPP


//============================================================================
//============================================================================
class dcutCuesData;
class dcutScriptObject;
class fsResourceTrackerData;
class nameString;


//============================================================================
//============================================================================
class dcutDataMgr
{
public:
	//--------------------------------------------------------------------
	//  Clean up
	//--------------------------------------------------------------------
	static void  Clear();

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
	//  Delete camera with given index
	//--------------------------------------------------------------------
	static void DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//  Get the camera
	//--------------------------------------------------------------------
	static dcutScriptObject* GetObject(int i_Index);

	//--------------------------------------------------------------------
	// return index of given Name, returns -1 if not found
	//--------------------------------------------------------------------
	static int GetIndexForObject( const nameString& i_Name );

	//--------------------------------------------------------------------
	//  get a list of resources.  the resources will be appended to the
	//	passed in list.
	//--------------------------------------------------------------------
	static void GetResourceList( fsResourceTrackerData& io_List );

};	// end of static class

