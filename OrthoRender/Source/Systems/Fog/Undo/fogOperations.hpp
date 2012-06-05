/*****************************************************************************
**	fogOperations.hpp
**
**	Utility for operations that are undoable in fog system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef FOG_OPERATIONS_HPP
#error fogOperations.hpp multiply included
#endif
#define FOG_OPERATIONS_HPP

#ifndef FOG_FOGDATA_HPP
#include "Systems/Fog/Data/fogFogData.hpp"
#endif

class nameString;
class fogScriptData;

namespace fogOperations
{
	//--------------------------------------------------------------------
	//  Add new light set to world
	//--------------------------------------------------------------------
	void  AddObject();
	void  AddObject(const fogScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Select light set with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Delete point light with given index
	//--------------------------------------------------------------------
	void  DeleteObject();

}	// end of namespace
