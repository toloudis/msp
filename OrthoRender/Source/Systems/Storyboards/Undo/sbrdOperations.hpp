/*****************************************************************************
**	sbrdOperations.hpp
**
**	Utility for doing operations that are undoable in SystemStoryboards
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SBRD_OPERATIONS_HPP
#error sbrdOperations.hpp multiply included
#endif
#define SBRD_OPERATIONS_HPP

#ifndef SBRD_SCRIPTDATA_HPP
#include "Systems/Storyboards/Data/sbrdScriptData.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class nameString;


//============================================================================
//============================================================================
namespace sbrdOperations
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void  AddStoryboard(const itString& i_Filename, bool i_bAddTo3DWorld = true);

	//------------------------------------------------------------------------
	//  Removes an item from the sbrd
	//------------------------------------------------------------------------
	void  RemoveStoryboard(int i_Index);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SwapStoryboards(int i_Index1, int i_Index2);

	//------------------------------------------------------------------------
	//  Add new item to sbrd
	//------------------------------------------------------------------------
	void  AddObject(const sbrdScriptData &i_Item);

	//--------------------------------------------------------------------
	//  Delete object with given index -- the index is irrelative since
	//	there can be only one storyboard object.
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//  Select sbrd with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Keep track of index of sbrd being edited so that calls to
	//  ChangeBillboardData affect the right one
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index);
}

