/*****************************************************************************
**	giActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GI_ACTUALOPERATIONS_HPP
#error giActualOperations.hpp multiply included
#endif
#define GI_ACTUALOPERATIONS_HPP

//============================================================================
//============================================================================
class giScriptData;

//============================================================================
//============================================================================
class giActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new point light to world
	//--------------------------------------------------------------------
	static int  AddObject(const giScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete point light with given index
	//--------------------------------------------------------------------
	static void  DeleteObject();

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	static void ChangeDriverData(const giScriptData& i_Data);

};	// end of static class

