/*****************************************************************************
**	trfnActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TRFN_ACTUALOPERATIONS_HPP
#error trfnActualOperations.hpp multiply included
#endif
#define TRFN_ACTUALOPERATIONS_HPP


//============================================================================
//============================================================================
class trfnScriptData;
class trfnData;
class nameString;


//============================================================================
//============================================================================
class trfnActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new transform to world
	//--------------------------------------------------------------------
	static int  AddObject(const trfnScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete transform with given index
	//--------------------------------------------------------------------
	static void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	static void  AddNodeToTransform(const nameString& i_SetName, 
							  const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	static void  RemoveNodeFromParent(const nameString& i_ObjectName);


};	// end of static class

