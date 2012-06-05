/*****************************************************************************
**	trfnOperations.hpp
**
**	Utility for operations that are undoable in trfn system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef TRFN_OPERATIONS_HPP
#error trfnOperations.hpp multiply included
#endif
#define TRFN_OPERATIONS_HPP

#include <map>

class trfnScriptData;
class xfrmTransformData;
class nameString;

namespace trfnOperations
{
	//--------------------------------------------------------------------
	//  Add new transform to world
	//--------------------------------------------------------------------
	void  AddObject();
	void  AddObject(const trfnScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Select transform with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Delete transform with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//  Make a clone of the object with given index
	//--------------------------------------------------------------------
	nameString  DuplicateObject(int i_Index,
								std::map<nameString, nameString> &o_DuplicateNameMap);
	
	//--------------------------------------------------------------------
	//	Create names for new object
	//--------------------------------------------------------------------
	void CreateNewObjectName(const nameString& i_Filename, nameString& o_NameString);

	//--------------------------------------------------------------------
	//	Add object to parent transform
	//--------------------------------------------------------------------
	void  AddNodeToTransform(const nameString& i_TransformName, 
							  const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Remove object from parent transform
	//--------------------------------------------------------------------
	void  RemoveNodeFromParent(const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//  Create new parent transform from selection
	//--------------------------------------------------------------------
	void  CreateTransformFromSelection();

	//--------------------------------------------------------------------
	//	Add selected objects to given parent transform
	//--------------------------------------------------------------------
	void  AddSelectedToTransform(const nameString& i_TransformName);

	//--------------------------------------------------------------------
	//	Remove selected objects from parent transforms (move to root)
	//--------------------------------------------------------------------
	void  RemoveSelectedFromParents();

}	// end of namespace
