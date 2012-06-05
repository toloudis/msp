/*****************************************************************************
**	lyrsOperations.hpp
**
**	Utility for operations that are undoable in lyrs system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef LYRS_OPERATIONS_HPP
#error lyrsOperations.hpp multiply included
#endif
#define LYRS_OPERATIONS_HPP

class lyrsData;
class lyerLayerData;
class nameString;

namespace lyrsOperations
{
	//--------------------------------------------------------------------
	//  Add new layer to world
	//--------------------------------------------------------------------
	void  AddObject();
	void  AddObject(const lyrsData& i_Data);

	//--------------------------------------------------------------------
	//  Select layer with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Delete layer with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index);

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddObjectToLayer(const nameString& i_SetName, 
							  const nameString& i_ObjectName);

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveObjectFromLayer(const nameString& i_SetName, 
								   const nameString& i_ObjectName);


}	// end of namespace
