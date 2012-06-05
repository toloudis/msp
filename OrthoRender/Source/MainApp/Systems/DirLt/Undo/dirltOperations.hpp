/*****************************************************************************
**	dirltOperations.hpp
**
**	Utility for operations that are undoable in dirlt system.
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
#ifdef DIRLT_OPERATIONS_HPP
#error dirltOperations.hpp multiply included
#endif
#define DIRLT_OPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class dirltScriptData;
class dirltData;
class nameString;
class maFloatRGBA;
class maRotation;


//============================================================================
//============================================================================
namespace dirltOperations
{
	//--------------------------------------------------------------------
	//	Ptlt operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp();

	//--------------------------------------------------------------------
	//  Add new dir light to world
	//--------------------------------------------------------------------
	void  AddObject();

	//--------------------------------------------------------------------
	//  Select dir light with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);

	//--------------------------------------------------------------------
	//  Delete dir light with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//  Make a clone of the dir light with given index
	//--------------------------------------------------------------------
	void  DuplicateDirLight(int i_Index);

	//--------------------------------------------------------------------
	//  Keep track of index of light being edited so that calls to
	//  ChangeLightData affect the right light
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index);

	//--------------------------------------------------------------------
	// Update individual basic properties
	//--------------------------------------------------------------------
	void ChangeBaseData(const dirltData& i_Data);

	//--------------------------------------------------------------------
	// Update individual dir light properties
	//--------------------------------------------------------------------
	void ChangeLightData(const dirltScriptData& i_Data);

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const dirltScriptData& i_Data);

	//--------------------------------------------------------------------
	// Change the Name
	//--------------------------------------------------------------------
	void ChangeName(const std::string& i_Name);

	//--------------------------------------------------------------------
	// Change the Position
	//--------------------------------------------------------------------
	void ChangePosition(const maPoint3d& i_Position);

	//--------------------------------------------------------------------
	// Change the Orientation
	//--------------------------------------------------------------------
	void ChangeOrientation(const maRotation& i_Orientation);

	//--------------------------------------------------------------------
	// ChangeColor
	//--------------------------------------------------------------------
	void ChangeColor(int i_Index, const maFloatRGBA& i_Color);

	//--------------------------------------------------------------------
	// ChangeEnabled
	//--------------------------------------------------------------------
	void ChangeEnabled(int i_Index, const bool i_Enabled);
}	// end of namespace
