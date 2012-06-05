/*****************************************************************************
**	envtOperations.hpp
**
**	Utility for operations that are undoable in envt system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef ENVT_OPERATIONS_HPP
#error envtOperations.hpp multiply included
#endif
#define ENVT_OPERATIONS_HPP

class api3dObjectSingle;
class envtScriptData;
class envtScriptObject;
class evmtEnvironmentData;
class nameObject;
class nameString;
class itString;

namespace envtOperations
{
	//--------------------------------------------------------------------
	//  Add new environment to world
	//--------------------------------------------------------------------
	void  AddObject();
	void  AddObject(const envtScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Select environment with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Delete point light with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//  Keep track of index of light being edited so that calls to
	//  ChangeLightData affect the right light
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index);

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const envtScriptData& i_Data);

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given environment
	//--------------------------------------------------------------------
	void  AddObjectToEnvironment(const nameString& i_EnvironmentName, 
							  const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given environment
	//--------------------------------------------------------------------
	void  RemoveObjectFromEnvironment(const nameString& i_EnvironmentName, 
								   const nameString& i_ObjectName);

}	// end of namespace
