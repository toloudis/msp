/*****************************************************************************
**	grupOperations.hpp
**
**	Utility for operations that are undoable in grup system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef GRUP_OPERATIONS_HPP
#error grupOperations.hpp multiply included
#endif
#define GRUP_OPERATIONS_HPP

class grupData;
class grpsGroupData;
class nameString;

namespace grupOperations
{
	//--------------------------------------------------------------------
	//  Add new group to world
	//--------------------------------------------------------------------
	void  AddObject();
	void  CreateGroupFromSelection();
	void  AddObject(const grupData& i_Data);

	//--------------------------------------------------------------------
	//  Select group with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Delete group with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddObjectToGroup(const nameString& i_SetName, 
							  const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveObjectFromGroup(const nameString& i_SetName, 
								   const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Select objects in a group
	//--------------------------------------------------------------------
	void  SelectGroupObjects(const nameString& i_GroupName);

}	// end of namespace
