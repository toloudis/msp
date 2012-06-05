/*****************************************************************************
**	chtrOperations.hpp
**
**	Utility for doing operations that are undoable in SystemCharacters
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_OPERATIONS_HPP
#error chtrOperations.hpp multiply included
#endif
#define CHTR_OPERATIONS_HPP

#ifndef CHTR_SCRIPTDATA_HPP
#include "Systems/Character/Data/chtrScriptData.hpp"
#endif


//============================================================================
//	forward references
//============================================================================


//============================================================================
//============================================================================
namespace chtrOperations
{
	//------------------------------------------------------------------------
	//  Add new item to character
	//------------------------------------------------------------------------
	void  AddObject(const chtrScriptData &i_Item);

	//------------------------------------------------------------------------
	//  Change data for a specific item
	//------------------------------------------------------------------------
	//void  EditCharacter(int i_Index, const chtrScriptData &i_Item);

	//------------------------------------------------------------------------
	//  Removes an item from the character
	//------------------------------------------------------------------------
	void  RemoveCharacter(int i_Index);

	//--------------------------------------------------------------------
	//  Make a clone of the Object with given index
	//--------------------------------------------------------------------
	void  DuplicateObject(int i_Index);

	//--------------------------------------------------------------------
	//  Reload geometry of character with given index
	//--------------------------------------------------------------------
	void  ReloadCharacter(int i_Index);

	//--------------------------------------------------------------------
	//  Select character with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//	Select object part, like surface or material
	//--------------------------------------------------------------------
	void SelectObjectPart(const nameString& i_Name, 
						  const std::string i_PartName, 
						  const std::string i_CategoryName, 
						  bool i_bAppend);

	//--------------------------------------------------------------------
	//	ActivateObjectPart from Placed, represents a double-click
	//		on a part item in the placed menu.
	//--------------------------------------------------------------------
	void ActivateObjectPart(const nameString& i_Name, 
							const std::string i_PartName, 
							const std::string i_CategoryName);

	//--------------------------------------------------------------------
	//	DeleteObjectPart from Placed, represents DELETE key or button
	//		when a part item is selected in the placed menu.
	//--------------------------------------------------------------------
	void DeleteObjectPart(const nameString& i_Name, 
						  const std::string i_PartName, 
						  const std::string i_CategoryName);

	//--------------------------------------------------------------------
	//  Keep track of index of character being edited so that calls to
	//  ChangeCharacterData affect the right one
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index);

	//--------------------------------------------------------------------
	// Update individual basic properties
	//--------------------------------------------------------------------
	void ChangeBaseData(const chtrData& i_Data);

	//--------------------------------------------------------------------
	// Update individual object properties
	//--------------------------------------------------------------------
	void ChangeCharacterData( const chtrScriptData& i_Data );

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const chtrScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Changes visible state of character with given index
	//--------------------------------------------------------------------
	void  SetEditorVisible(int i_Index, bool i_bVisible);

	//--------------------------------------------------------------------
	//  Change geometry of character with given name
	//--------------------------------------------------------------------
	void  ChangeFilename(const nameString& i_Name, const itString& i_Filename);

	//--------------------------------------------------------------------
	// Change the Name
	//--------------------------------------------------------------------
	void ChangeName(const std::string& i_Name);

	//--------------------------------------------------------------------
	// Set the weight for the expression with the given name
	//--------------------------------------------------------------------
	//void SetExpressionWeight(const std::string& i_Name, float i_Weight);
}
