/*****************************************************************************
**	cmraOperations.hpp
**
**	Utility for operations that are undoable in cmra system.
**
**	Everything outside of the "undo" folder should call a function
**	in this namespace to make any change to the data.  This will
**	make the change undoable and then call a function in
**	the "ActualOperations" namespace to make it really happen and to
**	update the GUI etc.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_OPERATIONS_HPP
#error cmraOperations.hpp multiply included
#endif
#define CMRA_OPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <string>


//============================================================================
//	forward references
//============================================================================
class cmraCameraData;
class cmraScriptData;
class maRotation;
class nameString;


//============================================================================
//============================================================================
namespace cmraOperations
{
	//--------------------------------------------------------------------
	//	Ptlt operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp();

	//--------------------------------------------------------------------
	//  Add new camera to world
	//--------------------------------------------------------------------
	void  AddObject(bool i_bOrthographic = false);

	//--------------------------------------------------------------------
	//  Select camera with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Select follow camera to given index
	//--------------------------------------------------------------------
	void  SelectFollowCamera(int i_Index);

	//--------------------------------------------------------------------
	//  Delete camera with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//  Make a clone of the Object with given index
	//--------------------------------------------------------------------
	void  DuplicateObject(int i_Index);

	//--------------------------------------------------------------------
	//  Keep track of index of camera being edited so that calls to
	//  ChangeCameraData affect the right camera
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index);

	//--------------------------------------------------------------------
	// Update individual basic properties
	//--------------------------------------------------------------------
	void ChangeBaseData(const cmraCameraData& i_Data);

	//--------------------------------------------------------------------
	// Update individual camera driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const cmraScriptData& i_Data);

	//--------------------------------------------------------------------
	// Change the Description
	//--------------------------------------------------------------------
	void ChangeDescription(const std::string& i_Description);

}	// end of namespace
