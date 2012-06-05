/*****************************************************************************
**	chtrActualOperations.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Undo/chtrActualOperations.hpp"

#include "Systems/Character/GUI/chtrDialogDataUtil.hpp"
#include "Systems/Character/Data/chtrDocumentChunk.hpp"

#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Core/fs/fsResourceTracker.hpp"


//--------------------------------------------------------------------
//  Add new Character to world
//--------------------------------------------------------------------
int  chtrActualOperations::AddObject(const chtrScriptData& i_Data)
{
	int index = chtrObjectMgr::AddObject(i_Data);
	if (index < 0) return index;	// if there is a problem loading the object, return immediately

	chtrDialogDataUtil::UpdateListDialog();
	chtrDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in chtrOperations
	chtrObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete Character with given index
//--------------------------------------------------------------------
void  chtrActualOperations::DeleteObject(int i_Index)
{
	//	remove the file from the resource tracker
	chtrScriptObject* pSO = chtrObjectMgr::GetObject(i_Index);
	if (pSO != 0)
	{
		fsResourceTracker::Remove(pSO->GetBaseData().m_Filename.GetValue());
		//fsResourceTracker::Debug_OutputList();
	}

	//	do the actual deletion
	chtrObjectMgr::DeleteObject(i_Index);
	chtrDialogDataUtil::UpdateListDialog();
	chtrDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
//	DeleteObjectPart from Placed, represents DELETE key or button
//		when a part item is selected in the placed menu.
//--------------------------------------------------------------------
void chtrActualOperations::DeleteObjectPart(const nameString& i_Name, 
					  const std::string i_PartName, 
					  const std::string i_CategoryName)
{
	int index = chtrObjectMgr::GetIndexForObject(i_Name);
	if (index >= 0)
	{
		chtrScriptObject *pCharacter = chtrObjectMgr::GetObject(index);
		pCharacter->DeleteObjectPart(i_PartName, i_CategoryName);

		chtrDialogDataUtil::UpdateListDialog();
		chtrDocumentChunk::ActiveDataChanged();
	}
}

//--------------------------------------------------------------------
// Update individual Character properties
//--------------------------------------------------------------------
void chtrActualOperations::SetData(int i_Index, const chtrScriptData& i_Data)
{
	chtrObjectMgr::SetScriptData(i_Index, i_Data);
	chtrDialogDataUtil::UpdateListDialog();			// because of name
	chtrDialogUtil::UpdateCharacterData(i_Index, i_Data.m_BaseData);

	chtrDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual driver properties
//--------------------------------------------------------------------
void chtrActualOperations::ChangeDriverData(int i_Index, const chtrScriptData& i_Data)
{
	// no need to notify chtrObjectMgr if there is no undo, 
	// this message is coming from the object directly

	chtrDialogUtil::UpdateCharacterData(i_Index, i_Data.m_BaseData);

	//if (chtrDocumentChunk::GetActiveChunk())
	//	chtrDocumentChunk::GetActiveChunk()->DataChanged();
}


//--------------------------------------------------------------------
// Set Name
//--------------------------------------------------------------------
void chtrActualOperations::SetName(int i_Index, const nameString& i_Name)
{
	// first get the latest data, then set the data
	//
	chtrData data = chtrObjectMgr::GetBaseData(i_Index);
	data.m_Name = i_Name;

	chtrObjectMgr::SetBaseData(i_Index, data);

	chtrDialogDataUtil::UpdateListDialog();				// because of name
	chtrDialogUtil::UpdateCharacterData(i_Index, data);
	chtrDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Set Filename
//--------------------------------------------------------------------
void chtrActualOperations::SetFilename(int i_Index, const itString& i_Filename)
{
	// first get the latest data, then set the data
	//
	chtrObjectMgr::ChangeFilename(i_Index, i_Filename);

	chtrDialogDataUtil::UpdateListDialog();
	chtrDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Set the weight for the expression with the given name
//--------------------------------------------------------------------
//void chtrActualOperations::SetExpressionWeight(int i_Index, const std::string& i_Name, float i_Weight)
//{
//	chtrObjectMgr::GetObject(i_Index)->SetExpressionWeight(i_Name, i_Weight);
//				
//	chtrDocumentChunk::ActiveDataChanged();
//}
