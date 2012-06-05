/*****************************************************************************
**	aoOperations.cpp
**
**	Utility for operations that are undoable in ao system
**
**	StudioGPU
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
	//void  AddObject()
	//{
	//	aoScriptData default_data;
	//	AddObject(default_data);
	//}
	//void  AddObject(const aoScriptData& i_Data)
	//{
	//	int index = aoActualOperations::AddObject(i_Data);
	//}

	//--------------------------------------------------------------------
	//  Select light set with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		sel3dMgr::CreateUndoOperation();
		aoObjectMgr::SelectObject();
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		sel3dMgr::CreateUndoOperation();
		aoObjectMgr::SelectObject(i_bAppend);
	}
	void  DeselectObject(const nameString& i_Name)
	{
		sel3dMgr::CreateUndoOperation();
		aoObjectMgr::DeselectObject();
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	//void  DeleteObject()
	//{
	//	aoActualOperations::DeleteObject();
	//}

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const aoScriptData& i_Data)
	{
		// we could do undo here
		aoActualOperations::ChangeDriverData(i_Data);
	}
}	// end of namespace
