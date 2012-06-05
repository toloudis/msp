/*****************************************************************************
**	grupActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef GRUP_ACTUALOPERATIONS_HPP
#error grupActualOperations.hpp multiply included
#endif
#define GRUP_ACTUALOPERATIONS_HPP

//============================================================================
//============================================================================
class grupScriptData;
class grupData;
class nameString;

//============================================================================
//============================================================================
class grupActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new group to world
	//--------------------------------------------------------------------
	static int  AddObject(const grupData& i_Data);

	//--------------------------------------------------------------------
	//  Delete group with given index
	//--------------------------------------------------------------------
	static void  DeleteObject(int i_Index);

};	// end of static class

