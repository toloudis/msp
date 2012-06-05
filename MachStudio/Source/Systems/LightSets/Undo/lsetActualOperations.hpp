/*****************************************************************************
**	lsetActualOperations.hpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
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

	//--------------------------------------------------------------------
	//	Add named light to named light set.
	//--------------------------------------------------------------------
	static void  AddLightToSet(const nameString& i_SetName, 
						const nameString& i_LightName);

	//--------------------------------------------------------------------
	//	Remove named light from named light set.
	//--------------------------------------------------------------------
	static void  RemoveLightFromSet(const nameString& i_SetName, 
							 const nameString& i_LightName);


	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	static void  AddObjectToLightSet(const nameString& i_SetName, 
							  const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	static void  RemoveObjectFromLightSet(const nameString& i_SetName, 
								   const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Add node to things that are lit by the given light set
	//--------------------------------------------------------------------
	static void  AddNodeToLightSet(const nameString& i_SetName, 
							const nameString& i_ObjectName,
							int i_NodeIndex);

	//--------------------------------------------------------------------
	//	Remove node from things that are lit by the given light set
	//--------------------------------------------------------------------
	static void  RemoveNodeFromLightSet(const nameString& i_SetName, 
								 const nameString& i_ObjectName,
								 int i_NodeIndex);
};	// end of static class

