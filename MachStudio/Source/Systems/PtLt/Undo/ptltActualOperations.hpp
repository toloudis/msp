/*****************************************************************************
**	ptltActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PTLT_ACTUALOPERATIONS_HPP
#error ptltActualOperations.hpp multiply included
#endif
#define PTLT_ACTUALOPERATIONS_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class ptltScriptData;
class ptltData;
class nameString;


//============================================================================
//============================================================================
class ptltActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new point light to world
	//--------------------------------------------------------------------
	static int  AddObject(const ptltScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete point light with given index
	//--------------------------------------------------------------------
	static void  DeleteObject(int i_Index);


};	// end of static class

