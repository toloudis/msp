/*****************************************************************************
**	prtclActualOperations.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Undo/prtclActualOperations.hpp"

#include "Systems/Particles/GUI/prtclDialogDataUtil.hpp"
#include "Systems/Particles/Data/prtclDocumentChunk.hpp"
#include "Systems/Particles/Timeline/prtclDriverCreator.hpp"

#include "Support/cmps/cmpsCompassMgr.hpp"
//#include "Support/tmln/tmlnChannelSet.hpp"
#include "Core/fs/fsResourceTracker.hpp"



//--------------------------------------------------------------------
//  Add new Particle to world
//--------------------------------------------------------------------
int  prtclActualOperations::AddObject(const prtclScriptData& i_Data)
{
	int index = prtclObjectMgr::AddObject(i_Data);
	if (index < 0) return index;	// if there is a problem loading the object, return immediately

	prtclDialogDataUtil::UpdateListDialog();
	prtclDocumentChunk::ActiveDataChanged();

	if (i_Data.m_Drivers.empty())
	{
		//	automatically create the Emit driver for this particle.
		//
		prtclScriptObject* pObj = prtclObjectMgr::GetObject(index);
		prtclDriverCreator::CreateDriverEmit( pObj );
	}

	// not sure if this should be here or in prtclOperations
	prtclObjectMgr::SelectObject(index);

	return index;
}

//--------------------------------------------------------------------
//  Delete Particle with given index
//--------------------------------------------------------------------
void  prtclActualOperations::DeleteObject(int i_Index)
{
	//	remove the file from the resource tracker
	prtclScriptObject* pSO = prtclObjectMgr::GetObject(i_Index);
	if (pSO != 0)
	{
		fsResourceTracker::Remove(pSO->GetBaseData().m_Filename.GetValue());
		//fsResourceTracker::Debug_OutputList();
	}

	//	do the actual deletion
	prtclObjectMgr::DeleteObject(i_Index);
	prtclDialogDataUtil::UpdateListDialog();
	prtclDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual base properties
//--------------------------------------------------------------------
void prtclActualOperations::SetBaseData(int i_Index, const prtclData& i_Data)
{
	prtclObjectMgr::SetBaseData(i_Index, i_Data);
	prtclDialogDataUtil::UpdateListDialog();			// because of name
	prtclDialogUtil::UpdateParticleData(i_Index, i_Data);

	prtclDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual Particle properties
//--------------------------------------------------------------------
void prtclActualOperations::SetData(int i_Index, const prtclScriptData& i_Data)
{
	prtclObjectMgr::SetScriptData(i_Index, i_Data);
	prtclDialogDataUtil::UpdateListDialog();		// because of name
	prtclDialogUtil::UpdateParticleData(i_Index, i_Data);

	prtclDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual driver properties
//--------------------------------------------------------------------
void prtclActualOperations::ChangeDriverData(int i_Index, const prtclScriptData& i_Data)
{
	// no need to notify prtclObjectMgr if there is no undo, 
	// this message is coming from the object directly

	prtclDialogUtil::UpdateParticleData(i_Index, i_Data);

	//	prtclDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Set Name
//--------------------------------------------------------------------
void prtclActualOperations::SetName(int i_Index, const nameString& i_Name)
{
	// first get the latest data, then set the position
	//
	prtclData data = prtclObjectMgr::GetBaseData(i_Index);
	data.m_Name = i_Name;

	prtclObjectMgr::SetBaseData(i_Index, data);

	// we don’t need to do the first call since the name isn’t changing.
	//
	prtclDialogDataUtil::UpdateListDialog();				// because of name
	prtclDialogUtil::UpdateParticleData(i_Index, data);

	prtclDocumentChunk::ActiveDataChanged();
}

