/*****************************************************************************
**	aoActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef AO_ACTUALOPERATIONS_HPP
#error aoActualOperations.hpp multiply included
#endif
#define AO_ACTUALOPERATIONS_HPP

//============================================================================
//============================================================================
class aoScriptData;

//============================================================================
//============================================================================
class aoActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new point light to world
	//--------------------------------------------------------------------
	static int  AddObject(const aoScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete point light with given index
	//--------------------------------------------------------------------
	static void  DeleteObject();

};	// end of static class

