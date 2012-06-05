/*****************************************************************************
**	prjltOperations.hpp
**
**	Utility for operations that are undoable in prjlt system.
**
**	Everything outside of the "undo" folder should call a function
**	in this namespace to make any change to the data.  This will
**	make the change undoable and then call a function in
**	the "ActualOperations" namespace to make it really happen and to
**	update the GUI etc.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRJLT_OPERATIONS_HPP
#error prjltOperations.hpp multiply included
#endif
#define PRJLT_OPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class prjltData;
class prjltScriptData;
class maRotation;
class nameString;


//============================================================================
//============================================================================
namespace prjltOperations
{
	//--------------------------------------------------------------------
	//	PrjLt operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp();

	//--------------------------------------------------------------------
	//  Add new projected light to world
	//--------------------------------------------------------------------
	void  AddObject();
	void  AddObject(const prjltScriptData& i_Data);

	//--------------------------------------------------------------------
	// Add projected light at current camera position
	//--------------------------------------------------------------------
	void AddLightAtCameraPosition();

	//--------------------------------------------------------------------
	//  Select projected light with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Delete projected light with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//  Make a clone of the object with given index
	//--------------------------------------------------------------------
	void  DuplicateObject(int i_Index);

	//--------------------------------------------------------------------
	//  Keep track of index of light being edited so that calls to
	//  ChangeLightData affect the right light
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index);

	//--------------------------------------------------------------------
	// Update individual basic properties
	//--------------------------------------------------------------------
	void ChangeBaseData(const prjltData& i_Data);

	//--------------------------------------------------------------------
	// Update individual projected light properties
	//--------------------------------------------------------------------
	void ChangeLightData(const prjltScriptData& i_Data);

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const prjltScriptData& i_Data);

}	// end of namespace
