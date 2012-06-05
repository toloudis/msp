/*****************************************************************************
**	envtActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Undo/envtActualOperations.hpp"

#include "Systems/Environments/GUI/envtDialogDataUtil.hpp"
#include "Systems/Environments/GUI/envtDialogUtil.hpp"
#include "Systems/Environments/Data/envtDocumentChunk.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"

//============================================================================
//============================================================================
//--------------------------------------------------------------------
//  Add new environment
//--------------------------------------------------------------------
int  envtActualOperations::AddObject(const envtScriptData& i_Data)
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	int index = envtObjectMgr::AddObject(i_Data);
	envtDialogDataUtil::UpdateListDialog();
	envtDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in envtOperations
	envtObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete environment with given index
//--------------------------------------------------------------------
void  envtActualOperations::DeleteObject(int i_Index)
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	envtObjectMgr::DeleteObject(i_Index);
	envtDialogDataUtil::UpdateListDialog();
	envtDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual driver properties
//--------------------------------------------------------------------
void envtActualOperations::ChangeDriverData(int i_Index, const envtScriptData& i_Data)
{
	// no need to notify envtObjectMgr if there is no undo, 
	// this message is coming from the object directly

	envtDialogUtil::UpdateEnvironmentData(i_Index, i_Data);
	envtDocumentChunk::ActiveDataChanged();;
}

//--------------------------------------------------------------------
//	Add object to things that are lit by the given environment
//--------------------------------------------------------------------
void  envtActualOperations::AddObjectToEnvironment(const nameString& i_EnvironmentName, 
						  const nameString& i_ObjectName)
{
	evmtEnvironmentMgr::AddObjectToEnvironment(i_EnvironmentName, i_ObjectName);
	envtDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
//	Remove object from things that are lit by the given environment
//--------------------------------------------------------------------
void  envtActualOperations::RemoveObjectFromEnvironment(const nameString& i_EnvironmentName, 
							   const nameString& i_ObjectName)
{
	evmtEnvironmentMgr::RemoveObjectFromEnvironment(i_EnvironmentName, i_ObjectName);
	envtDocumentChunk::ActiveDataChanged();
}
