/*****************************************************************************
**	ptltOperations.hpp
**
**	Utility for operations that are undoable in ptlt system.
**
**	Everything outside of the "undo" folder should call a function
**	in this namespace to make any change to the data.  This will
**	make the change undoable and then call a function in
**	the "ActualOperations" namespace to make it really happen and to
**	update the GUI etc.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PTLT_OPERATIONS_HPP
#error ptltOperations.hpp multiply included
#endif
#define PTLT_OPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class ptltData;
class ptltScriptData;
class nameString;


//============================================================================
//============================================================================
namespace ptltOperations
{
	//--------------------------------------------------------------------
	//	Ptlt operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp();

	//--------------------------------------------------------------------
	//  Add new point light to world
	//--------------------------------------------------------------------
	void  AddObject();
	void  AddObject(const ptltScriptData& i_Data);

	//--------------------------------------------------------------------
	// Add point light at current camera position
	//--------------------------------------------------------------------
	void AddLightAtCameraPosition();

	//--------------------------------------------------------------------
	//  Select point light with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Delete point light with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//  Make a clone of the object with given index
	//--------------------------------------------------------------------
	nameString DuplicateObject(int i_Index);

	//--------------------------------------------------------------------
	//  Keep track of index of light being edited so that calls to
	//  ChangeLightData affect the right light
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index);

	//--------------------------------------------------------------------
	//	Create names for new object
	//--------------------------------------------------------------------
	void CreateNewObjectName(const nameString& i_Filename, nameString& o_NameString);

}	// end of namespace
