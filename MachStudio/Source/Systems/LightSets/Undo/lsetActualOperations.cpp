/*****************************************************************************
**	lsetActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Undo/lsetActualOperations.hpp"

#include "Systems/LightSets/GUI/lsetDialogDataUtil.hpp"
#include "Systems/LightSets/GUI/lsetDialogUtil.hpp"
#include "Systems/LightSets/Data/lsetDocumentChunk.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"
\
//============================================================================
//============================================================================
namespace
{
	// all operations need to set data changed
	void set_data_changed()
	{
		lsetDocumentChunk::ActiveDataChanged();
	}

}

//--------------------------------------------------------------------
//  Add new light set
//--------------------------------------------------------------------
int  lsetActualOperations::AddObject(const lsetScriptData& i_Data)
{
	int index = lsetObjectMgr::AddObject(i_Data);
	lsetDialogDataUtil::UpdateListDialog();
	lsetDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in lsetOperations
	lsetObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete light set with given index
//--------------------------------------------------------------------
void  lsetActualOperations::DeleteObject(int i_Index)
{
	lsetObjectMgr::DeleteObject(i_Index);
	lsetDialogDataUtil::UpdateListDialog();
	lsetDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
//	Add named light to named light set.
//--------------------------------------------------------------------
void  lsetActualOperations::AddLightToSet(const nameString& i_SetName, 
					const nameString& i_LightName)
{
	ltstLightSetMgr::AddLightToSet(i_SetName, i_LightName);
	lsetDialogDataUtil::UpdateSetRelationships();
	set_data_changed();
}

//--------------------------------------------------------------------
//	Remove named light from named light set.
//--------------------------------------------------------------------
void  lsetActualOperations::RemoveLightFromSet(const nameString& i_SetName, 
						 const nameString& i_LightName)
{
	ltstLightSetMgr::RemoveLightFromSet(i_SetName, i_LightName);
	lsetDialogDataUtil::UpdateSetRelationships();
	set_data_changed();
}


//--------------------------------------------------------------------
//	Add object to things that are lit by the given light set
//--------------------------------------------------------------------
void  lsetActualOperations::AddObjectToLightSet(const nameString& i_SetName, 
						  const nameString& i_ObjectName)
{
	ltstLightSetMgr::AddObjectToLightSet(i_SetName, i_ObjectName);
	set_data_changed();
}

//--------------------------------------------------------------------
//	Remove object from things that are lit by the given light set
//--------------------------------------------------------------------
void  lsetActualOperations::RemoveObjectFromLightSet(const nameString& i_SetName, 
							   const nameString& i_ObjectName)
{
	ltstLightSetMgr::RemoveObjectFromLightSet(i_SetName, i_ObjectName);
	set_data_changed();
}

//--------------------------------------------------------------------
//	Add node to things that are lit by the given light set
//--------------------------------------------------------------------
void  lsetActualOperations::AddNodeToLightSet(const nameString& i_SetName, 
						const nameString& i_ObjectName,
						int i_NodeIndex)
{
	ltstLightSetMgr::AddNodeToLightSet(i_SetName, i_ObjectName, i_NodeIndex);
	set_data_changed();
}

//--------------------------------------------------------------------
//	Remove node from things that are lit by the given light set
//--------------------------------------------------------------------
void  lsetActualOperations::RemoveNodeFromLightSet(const nameString& i_SetName, 
							 const nameString& i_ObjectName,
							 int i_NodeIndex)
{
	ltstLightSetMgr::RemoveNodeFromLightSet(i_SetName, i_ObjectName, i_NodeIndex);
	set_data_changed();
}
