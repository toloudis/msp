/*****************************************************************************
**	setsDataMgr.hpp
**
**	Keeps track of the current displayed set items
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SETS_DATAMGR_HPP
#error setsDataMgr.hpp multiply included
#endif
#define SETS_DATAMGR_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class setsData;
class setsListData;
class setsScriptData;
class fsLocator;
class geoPickRay;
class setsObject;
class pick3dPickObject;


//============================================================================
//============================================================================
namespace setsDataMgr
{
	// EventCallback when any data in this manager changes
	class DataChangedCallback
	{
		public:
			virtual void DataChanged() = 0;
	};

	//--------------------------------------------------------------------
	// Clear all items
	//--------------------------------------------------------------------
	void  Clear();

	//--------------------------------------------------------------------
	//  Add new item to set
	//--------------------------------------------------------------------
	void  AddSetItem(setsScriptData &i_Item);

	//--------------------------------------------------------------------
	//  Removes an item from the set
	//--------------------------------------------------------------------
	void  RemoveSetItem(const setsScriptData &i_Item);
	void  RemoveSetItem(int i_Index);

	//--------------------------------------------------------------------
	//  Return number of set items
	//--------------------------------------------------------------------
	int  GetNumSetItems();

	//--------------------------------------------------------------------
	//  Get data for a specfic item
	//--------------------------------------------------------------------
	const setsData& GetBaseData(int i_Index);
	const setsScriptData& GetItemData(int i_Index);

	//--------------------------------------------------------------------
	// return index of given Name, returns -1 if not found
	//--------------------------------------------------------------------
	int GetIndexForItem( const nameString& i_Name );

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	const setsListData& GetData();
	void SetData(setsListData &i_Data);

	//--------------------------------------------------------------------
	// Append objects from this data into list, not allowing duplicates
	//--------------------------------------------------------------------
	void MergeData(setsListData &i_Data);

	//--------------------------------------------------------------------
	//  Changes visible state of set item with given index
	//--------------------------------------------------------------------
	void  SetEditorVisible(int i_Index, bool i_bVisible);

	//--------------------------------------------------------------------
	//  Returns the visible state of set item with given index
	//--------------------------------------------------------------------
	bool  GetEditorVisible(int i_Index);

	//--------------------------------------------------------------------
	// Make sure that all geometry is visible for rendering
	//--------------------------------------------------------------------
	void ConfirmGeometryVisible();

	//--------------------------------------------------------------------
	// This callback will be called when the data changes
	//--------------------------------------------------------------------
	//void AddDataChangedCallback(DataChangedCallback *i_Func);
	//void RemoveDataChangedCallback(DataChangedCallback *i_Func);

	//--------------------------------------------------------------------
	//	Ray pick occluders returning a modified tVal
	//--------------------------------------------------------------------
	bool OccluderPick(geoPickRay& i_Ray, float &o_tVal);

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	setsObject* GetObject(int index);

	//--------------------------------------------------------------------
	//	Select Object from Placed
	//--------------------------------------------------------------------
	void SelectObject(int i_Index, bool i_bAppend);

	//--------------------------------------------------------------------
	//	Remove Object from Selection
	//--------------------------------------------------------------------
	void  DeselectObject(int i_Index);

}	// end of namespace
