/*****************************************************************************
**	chtrActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_ACTUALOPERATIONS_HPP
#error chtrActualOperations.hpp multiply included
#endif
#define CHTR_ACTUALOPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <string>

//============================================================================
//============================================================================
class nameString;
class chtrData;
class chtrScriptData;
class maRotation;
class fsLocator;


//============================================================================
//============================================================================
class chtrActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new Character to world
	//--------------------------------------------------------------------
	static int  AddObject(const chtrScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete Character with specific data or a given index
	//--------------------------------------------------------------------
	static void  DeleteObject(const chtrScriptData& i_Data);
	static void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//	DeleteObjectPart from Placed, represents DELETE key or button
	//		when a part item is selected in the placed menu.
	//--------------------------------------------------------------------
	static void DeleteObjectPart(const nameString& i_Name, 
						  const std::string i_PartName, 
						  const std::string i_CategoryName);

	//--------------------------------------------------------------------
	// Update individual Character properties
	//--------------------------------------------------------------------
	static void SetData(int i_Index, const chtrScriptData& i_Data);

	//--------------------------------------------------------------------
	// Update individual driver chtrerties
	//--------------------------------------------------------------------
	static void ChangeDriverData(int i_Index, const chtrScriptData& i_Data);

	//--------------------------------------------------------------------
	// Set Name
	//--------------------------------------------------------------------
	static void SetName(int i_Index, const nameString& i_Name);

	//--------------------------------------------------------------------
	// Set Filename
	//--------------------------------------------------------------------
	static void SetFilename(int i_Index, const fsLocator& i_Filename);

	//--------------------------------------------------------------------
	// Set the weight for the expression with the given name
	//--------------------------------------------------------------------
	//static void SetExpressionWeight(int i_Index, const std::string& i_Name, float i_Weight);

};	// end of static class
