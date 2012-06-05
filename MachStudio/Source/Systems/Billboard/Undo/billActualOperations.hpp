/*****************************************************************************
**	billActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef BILL_ACTUALOPERATIONS_HPP
#error billActualOperations.hpp multiply included
#endif
#define BILL_ACTUALOPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

//============================================================================
//============================================================================
class billData;
class billScriptData;
class nameString;
class maRotation;


//============================================================================
//============================================================================
class billActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new Billboard to world
	//--------------------------------------------------------------------
	static int  AddObject(const billScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete Billboard with specific data or a given index
	//--------------------------------------------------------------------
	static void  DeleteObject(const billScriptData& i_Data);
	static void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	static void ChangeDriverData(int i_Index, const billScriptData& i_Data);

};	// end of static class
