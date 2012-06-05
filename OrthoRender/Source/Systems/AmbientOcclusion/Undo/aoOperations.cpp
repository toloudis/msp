/*****************************************************************************
**	aoOperations.cpp
**
**	Utility for operations that are undoable in ao system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/AmbientOcclusion/Undo/aoOperations.hpp"

#include "Systems/AmbientOcclusion/Object/aoObjectMgr.hpp"
#include "Systems/AmbientOcclusion/Undo/aoActualOperations.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"

namespace aoOperations
{
	//--------------------------------------------------------------------
	//  Add new light set
	//--------------------------------------------------------------------
	void  AddObject()
	{
		aoScriptData default_data;
		AddObject(default_data);
	}
	void  AddObject(const aoScriptData& i_Data)
	{
		int index = aoActualOperations::AddObject(i_Data);
	}

	//--------------------------------------------------------------------
	//  Select light set with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		aoObjectMgr::SelectObject();
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		aoObjectMgr::SelectObject(i_bAppend);
	}
	void  DeselectObject(const nameString& i_Name)
	{
		aoObjectMgr::DeselectObject();
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	void  DeleteObject()
	{
		aoActualOperations::DeleteObject();
	}

}	// end of namespace
