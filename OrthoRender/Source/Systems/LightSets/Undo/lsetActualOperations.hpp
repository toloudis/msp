/*****************************************************************************
**	lsetActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef LSET_ACTUALOPERATIONS_HPP
#error lsetActualOperations.hpp multiply included
#endif
#define LSET_ACTUALOPERATIONS_HPP

//============================================================================
//============================================================================
class lsetScriptData;
class lsetData;
class nameString;

//============================================================================
//============================================================================
class lsetActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new point light to world
	//--------------------------------------------------------------------
	static int  AddObject(const lsetScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete point light with given index
	//--------------------------------------------------------------------
	static void  DeleteObject(int i_Index);

};	// end of static class

