/*****************************************************************************
**	aoOperations.hpp
**
**	Utility for operations that are undoable in ao system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef AO_OPERATIONS_HPP
#error aoOperations.hpp multiply included
#endif
#define AO_OPERATIONS_HPP

#ifndef AO_AODATA_HPP
#include "Systems/AmbientOcclusion/Data/aoAOData.hpp"
#endif

class nameString;
class aoScriptData;

namespace aoOperations
{
	//--------------------------------------------------------------------
	//  Add new light set to world
	//--------------------------------------------------------------------
	void  AddObject();
	void  AddObject(const aoScriptData& i_Data);

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
