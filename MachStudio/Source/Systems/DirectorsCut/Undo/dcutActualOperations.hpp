/*****************************************************************************
**	dcutActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef DCUT_ACTUALOPERATIONS_HPP
#error dcutActualOperations.hpp multiply included
#endif
#define DCUT_ACTUALOPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class dcutScriptData;
class dcutCueData;
class nameString;
class maRotation;


//============================================================================
//============================================================================
class dcutActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new camera to world
	//--------------------------------------------------------------------
	static int  AddObject(const dcutScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete camera with given index
	//--------------------------------------------------------------------
	static void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	// Update individual base properties
	//--------------------------------------------------------------------
	static void SetBaseData(int i_Index, const dcutCueData& i_Data);

	//--------------------------------------------------------------------
	// Update individual camera properties
	//--------------------------------------------------------------------
	static void SetData(int i_Index, const dcutScriptData& i_Data );

	//--------------------------------------------------------------------
	//	Select the camera and set the datadata (no matter what)
	//--------------------------------------------------------------------
	static void UpdateCameraData( int i_Index );

	//--------------------------------------------------------------------
	// Update individual camera driver properties
	//--------------------------------------------------------------------
	static void ChangeDriverData(int i_Index, const dcutScriptData& i_Data);

	//--------------------------------------------------------------------
	// Set Description
	//--------------------------------------------------------------------
	//static void SetDescription(int i_Index, const nameString& i_Description);

};	// end of static class

