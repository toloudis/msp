/*****************************************************************************
**	fogOperations.cpp
**
**	Utility for operations that are undoable in fog system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Fog/Undo/fogOperations.hpp"

#include "Systems/Fog/Object/fogObjectMgr.hpp"
#include "Systems/Fog/Undo/fogActualOperations.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"

namespace fogOperations
{
	//--------------------------------------------------------------------
	//  Add new light set
	//--------------------------------------------------------------------
	//void  AddObject()
	//{
	//	fogScriptData default_data;
	//	AddObject(default_data);
	//}
	//void  AddObject(const fogScriptData& i_Data)
	//{
	//	int index = fogActualOperations::AddObject(i_Data);
	//}

	//--------------------------------------------------------------------
	//  Select light set with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		sel3dMgr::CreateUndoOperation();
		fogObjectMgr::SelectObject();
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		sel3dMgr::CreateUndoOperation();
		fogObjectMgr::SelectObject(i_bAppend);
	}
	void  DeselectObject(const nameString& i_Name)
	{
		sel3dMgr::CreateUndoOperation();
		fogObjectMgr::DeselectObject();
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	//void  DeleteObject()
	//{
	//	fogActualOperations::DeleteObject();
	//}

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const fogScriptData& i_Data)
	{
		// we could do undo here
		fogActualOperations::ChangeDriverData(i_Data);
	}

}	// end of namespace
