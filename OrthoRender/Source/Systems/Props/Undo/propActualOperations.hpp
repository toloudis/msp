/*****************************************************************************
**	propActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PROP_ACTUALOPERATIONS_HPP
#error propActualOperations.hpp multiply included
#endif
#define PROP_ACTUALOPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class nameString;
class propData;
class propScriptData;
class maRotation;


//============================================================================
//============================================================================
class propActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new Prop to world
	//--------------------------------------------------------------------
	static int  AddObject(const propScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete Prop with specific data or a given index
	//--------------------------------------------------------------------
	static void  DeleteObject(const propScriptData& i_Data);
	static void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	static void ChangeDriverData(int i_Index, const propScriptData& i_Data);
};	// end of static class
