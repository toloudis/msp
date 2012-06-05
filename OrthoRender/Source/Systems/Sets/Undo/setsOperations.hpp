/*****************************************************************************
**	setsOperations.hpp
**
**	Utility for doing operations that are undoable in SystemSets
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SETS_OPERATIONS_HPP
#error setsOperations.hpp multiply included
#endif
#define SETS_OPERATIONS_HPP


//============================================================================
// Forward References
//============================================================================
class setsListData;
class setsScriptData;


//============================================================================
//============================================================================
namespace setsOperations
{
	//--------------------------------------------------------------------
	//  Add new item to set
	//--------------------------------------------------------------------
	void  AddSetItem(setsScriptData &i_Item);

	//--------------------------------------------------------------------
	//  Change data for a specific item
	//--------------------------------------------------------------------
	//void  EditSetItem(int i_Index, const setsScriptData &i_Item);

	//--------------------------------------------------------------------
	//  Removes an item from the set
	//--------------------------------------------------------------------
	void  RemoveSetItem(int i_Index);

	//--------------------------------------------------------------------
	//  Changes visible state of set item with given index
	//--------------------------------------------------------------------
	void  SetEditorVisible(int i_Index, bool i_bVisible);


}	// end of namespace
