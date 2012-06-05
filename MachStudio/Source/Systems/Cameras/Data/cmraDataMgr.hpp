/*****************************************************************************
**	cmraDataMgr.hpp
**
**	Adapter object for DocumentChunk template...
**	manages split of cmraCamerasData between ObjectMgr and Cue data
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_DATAMGR_HPP
#error cmraDataMgr.hpp multiply included
#endif
#define CMRA_DATAMGR_HPP

//============================================================================
//============================================================================
class cmraCamerasData;
class cmraScriptObject;
class fsResourceTrackerData;
class nameString;


//============================================================================
//============================================================================
class cmraDataMgr
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
	static cmraScriptObject* GetObject(int i_Index);

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

