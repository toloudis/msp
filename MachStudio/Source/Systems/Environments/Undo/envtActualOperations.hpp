/*****************************************************************************
**	envtActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_ACTUALOPERATIONS_HPP
#error envtActualOperations.hpp multiply included
#endif
#define ENVT_ACTUALOPERATIONS_HPP

//============================================================================
//============================================================================
class envtScriptData;
class envtData;
class nameString;

//============================================================================
//============================================================================
class envtActualOperations
{
public:
	//--------------------------------------------------------------------
	//  Add new point light to world
	//--------------------------------------------------------------------
	static int  AddObject(const envtScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Delete point light with given index
	//--------------------------------------------------------------------
	static void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	static void ChangeDriverData(int i_Index, const envtScriptData& i_Data);

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given environment
	//--------------------------------------------------------------------
	static void  AddObjectToEnvironment(const nameString& i_EnvironmentName, 
							  const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given environment
	//--------------------------------------------------------------------
	static void  RemoveObjectFromEnvironment(const nameString& i_EnvironmentName, 
								   const nameString& i_ObjectName);

};	// end of static class

