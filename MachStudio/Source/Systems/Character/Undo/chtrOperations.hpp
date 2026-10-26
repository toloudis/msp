/*****************************************************************************
**	chtrOperations.hpp
**
**	Utility for doing operations that are undoable in SystemCharacters
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_OPERATIONS_HPP
#error chtrOperations.hpp multiply included
#endif
#define CHTR_OPERATIONS_HPP

#ifndef CHTR_SCRIPTDATA_HPP
#include "Systems/Character/Data/chtrScriptData.hpp"
#endif

#include <functional>
#include <string>


//============================================================================
//	forward references
//============================================================================


//============================================================================
//============================================================================
namespace chtrOperations
{
	//------------------------------------------------------------------------
	// Let user search for objects to load.
	//------------------------------------------------------------------------
	void BrowseLoadObjects();

	//------------------------------------------------------------------------
	// The code BrowseLoadObjects offers each file to first: a scene file's
	// objects are added by it (Features/Import's imptSceneAppend, which can
	// reach every System), and it returns false for a file it doesn't add,
	// which then loads as a character. i_FilePatterns ("*.a;*.b") are added
	// to the browse filter. An empty function removes it.
	//------------------------------------------------------------------------
	void SetFileAppender(std::function<bool (const fsLocator&)> i_Appender, const std::string& i_FilePatterns);

	//------------------------------------------------------------------------
	//  LoadAnimation onto the character with the given index.
	//------------------------------------------------------------------------
	void LoadAnimation(int i_Index, const fsLocator &i_AnimLocator);

	//------------------------------------------------------------------------
	//  Add new item to character
	//------------------------------------------------------------------------
	// returns the name the new object got, empty if it couldn't be added
	nameString  AddObject(const chtrScriptData &i_Item);

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
	nameString  DuplicateObject(int i_Index);

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
	void ToggleObjectPartSelection(const nameString& i_Name, 
						  const std::string i_PartName, 
						  const std::string i_CategoryName);

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
	// Set object active state in the render layer
	//--------------------------------------------------------------------
	void SetActiveRenderLayer(int i_Index, bool i_bActive);

	//--------------------------------------------------------------------
	//  Changes visible state of character with given index
	//--------------------------------------------------------------------
	void  SetEditorVisible(int i_Index, bool i_bVisible);

	//--------------------------------------------------------------------
	//  Change geometry of character with given name
	//--------------------------------------------------------------------
	void  ChangeFilename(const nameString& i_Name, const fsLocator& i_Filename);

	//--------------------------------------------------------------------
	// Change the Name
	//--------------------------------------------------------------------
	void ChangeName(const std::string& i_Name);

	//--------------------------------------------------------------------
	// Set the weight for the expression with the given name
	//--------------------------------------------------------------------
	//void SetExpressionWeight(const std::string& i_Name, float i_Weight);
	//--------------------------------------------------------------------
	//	Create names for new object
	//--------------------------------------------------------------------
	void CreateNewObjectName(const nameString& i_Filename, nameString& o_NameString);
}
