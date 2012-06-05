/*****************************************************************************
**	giOperations.hpp
**
**	Utility for operations that are undoable in gi system
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GI_OPERATIONS_HPP
#error giOperations.hpp multiply included
#endif
#define GI_OPERATIONS_HPP

#ifndef GI_GIDATA_HPP
#include "Systems/GlobalIllumination/Data/giGIData.hpp"
#endif

class nameString;
class giScriptData;

namespace giOperations
{
	//--------------------------------------------------------------------
	//  Add new light set to world
	//--------------------------------------------------------------------
	//void  AddObject();
	//void  AddObject(const aoScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Select light set with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index);
	void  SelectObject(const nameString& i_Name, bool i_bAppend = false);
	void  DeselectObject(const nameString& i_Name);

	//--------------------------------------------------------------------
	//  Delete point light with given index
	//--------------------------------------------------------------------
	//void  DeleteObject();

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const giScriptData& i_Data);
}	// end of namespace
