/*****************************************************************************
**	lyrsActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef LYRS_ACTUALOPERATIONS_HPP
#error lyrsActualOperations.hpp multiply included
#endif
#define LYRS_ACTUALOPERATIONS_HPP


//============================================================================
//============================================================================
class lyrsScriptData;
class lyrsData;
class nameString;


//============================================================================
//============================================================================
class lyrsActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new layer to world
	//--------------------------------------------------------------------
	static int  AddObject(const lyrsData& i_Data);

	//--------------------------------------------------------------------
	//  Delete layer with given index
	//--------------------------------------------------------------------
	static void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	static void  AddObjectToLayer(const nameString& i_SetName, 
							  const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	static void  RemoveObjectFromLayer(const nameString& i_SetName, 
								   const nameString& i_ObjectName);


};	// end of static class

