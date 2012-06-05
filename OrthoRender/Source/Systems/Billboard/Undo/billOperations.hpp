/*****************************************************************************
**	billOperations.hpp
**
**	Utility for doing operations that are undoable in SystemBillboards
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef BILL_OPERATIONS_HPP
#error billOperations.hpp multiply included
#endif
#define BILL_OPERATIONS_HPP

#ifndef BILL_SCRIPTDATA_HPP
#include "Systems/Billboard/Data/billScriptData.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class nameString;


//============================================================================
//============================================================================
namespace billOperations
{
	//------------------------------------------------------------------------
	//  Add new item to bill
	//------------------------------------------------------------------------
	void  AddObject(const billScriptData &i_Item);

	//------------------------------------------------------------------------
	//  Removes an item from the bill
	//------------------------------------------------------------------------
	void  RemoveBillboard(int i_Index);

	//------------------------------------------------------------------------
	//  Duplicates an item from the bill
	//------------------------------------------------------------------------
	void  DuplicateBillboard(int i_Index);

	//--------------------------------------------------------------------
	//  Select bill with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Keep track of index of bill being edited so that calls to
	//  ChangeBillboardData affect the right one
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index);
}

