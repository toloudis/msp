/*****************************************************************************
**	giOperations.cpp
**
**	Utility for operations that are undoable in gi system
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/


#include "Systems/GlobalIllumination/Undo/giOperations.hpp"

#include "Systems/GlobalIllumination/Object/giObjectMgr.hpp"
#include "Systems/GlobalIllumination/Undo/giActualOperations.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"

namespace giOperations
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
		giObjectMgr::SelectObject();
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		sel3dMgr::CreateUndoOperation();
		giObjectMgr::SelectObject(i_bAppend);
	}
	void  DeselectObject(const nameString& i_Name)
	{
		sel3dMgr::CreateUndoOperation();
		giObjectMgr::DeselectObject();
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
	void ChangeDriverData(const giScriptData& i_Data)
	{
		// we could do undo here
		giActualOperations::ChangeDriverData(i_Data);
	}
}	// end of namespace
