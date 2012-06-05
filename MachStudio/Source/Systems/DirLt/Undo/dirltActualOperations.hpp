/*****************************************************************************
**	dirltActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef DIRLT_ACTUALOPERATIONS_HPP
#error dirltActualOperations.hpp multiply included
#endif
#define DIRLT_ACTUALOPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class dirltData;
class dirltScriptData;
class nameString;
class maFloatRGBA;
class maRotation;


//============================================================================
//============================================================================
class dirltActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new dir light to world
	//--------------------------------------------------------------------
	static int  AddObject(const dirltScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete dir light with given index
	//--------------------------------------------------------------------
	static void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	// Update individual base properties
	//--------------------------------------------------------------------
	static void SetBaseData(int i_Index, const dirltData& i_Data);

	//--------------------------------------------------------------------
	// Update individual dir light properties
	//--------------------------------------------------------------------
	static void SetData(int i_Index, const dirltScriptData& i_Data);

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	static void ChangeDriverData(int i_Index, const dirltScriptData& i_Data);

	//--------------------------------------------------------------------
	// Set Name
	//--------------------------------------------------------------------
	static void SetName(int i_Index, const nameString& i_Name);

	//--------------------------------------------------------------------
	// Set Position
	//--------------------------------------------------------------------
	static void SetPosition(int i_Index, const maPoint3d& i_Position);

	//--------------------------------------------------------------------
	// Set Orientation
	//--------------------------------------------------------------------
	static void SetOrientation(int i_Index, const maRotation& i_Orientation);

	//--------------------------------------------------------------------
	// SetColor
	//--------------------------------------------------------------------
	static void SetColor(int i_Index, const maFloatRGBA& i_Color);

	//--------------------------------------------------------------------
	// SetEnabled
	//--------------------------------------------------------------------
	static void SetEnabled(int i_Index, const bool i_Enabled);

};	// end of static class

