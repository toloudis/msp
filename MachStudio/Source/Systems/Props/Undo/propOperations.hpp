/*****************************************************************************
**	propOperations.hpp
**
**	Utility for doing operations that are undoable in SystemProps
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PROP_OPERATIONS_HPP
#error propOperations.hpp multiply included
#endif
#define PROP_OPERATIONS_HPP

#ifndef PROP_SCRIPTDATA_HPP
#include "Systems/Props/Data/propScriptData.hpp"
#endif

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif


//============================================================================
//	forward references
//============================================================================


//============================================================================
//============================================================================
namespace propOperations
{
	//------------------------------------------------------------------------
	//  Add new item to prop
	//------------------------------------------------------------------------
	void  AddObject(const propScriptData &i_Item);

	//------------------------------------------------------------------------
	//  Removes an item from the prop
	//------------------------------------------------------------------------
	void  RemoveProp(int i_Index);

	//--------------------------------------------------------------------
	//  Make a clone of the Object with given index
	//--------------------------------------------------------------------
	void  DuplicateObject(int i_Index);

	//--------------------------------------------------------------------
	//  Select prop with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//	Select object part, like surface or material
	//--------------------------------------------------------------------
	void SelectObjectPart(const nameString& i_Name, 
						  const std::string i_PartName, 
						  const std::string i_CategoryName, 
						  bool i_bAppend);

	//--------------------------------------------------------------------
	//  Keep track of index of prop being edited so that calls to
	//  ChangePropData affect the right one
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index);

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const propScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Changes visible state of prop with given index
	//--------------------------------------------------------------------
	void  SetEditorVisible(int i_Index, bool i_bVisible);
}

