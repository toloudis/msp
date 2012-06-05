/*****************************************************************************
**	prjltActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRJLT_ACTUALOPERATIONS_HPP
#error prjltActualOperations.hpp multiply included
#endif
#define PRJLT_ACTUALOPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class prjltData;
class prjltScriptData;
class nameString;
class maRotation;


//============================================================================
//============================================================================
class prjltActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new projected light to world
	//--------------------------------------------------------------------
	static int  AddObject(const prjltScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete projected light with given index
	//--------------------------------------------------------------------
	static void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	// Update individual base properties
	//--------------------------------------------------------------------
	static void SetBaseData(int i_Index, const prjltData& i_Data);

	//--------------------------------------------------------------------
	// Update individual projected light properties
	//--------------------------------------------------------------------
	static void SetData(int i_Index, const prjltScriptData& i_Data);

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	static void ChangeDriverData(int i_Index, const prjltScriptData& i_Data);

};	// end of static class

