/*****************************************************************************
**	mtrlSetOperation.hpp
**
**	Undoable operation for changing material data as a whole. 
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/GUI/mtrlSetOperation.hpp"

#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Core/rel/relObject.hpp"


//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
mtrlSetOperation::mtrlSetOperation( const char* i_DisplayName,
								  mtrlScriptObject *i_pObject,
								  int i_MaterialIndex,
								  const mdlMaterialInfo& i_OldData,
								  bool i_bUpdateProperties)
:	m_MaterialIndex(i_MaterialIndex), 
	m_bUpdateProperties(i_bUpdateProperties), 
	m_DataBackup( i_OldData ),
	m_DisplayName( i_DisplayName )
{
	relObject *pRelObject = dynamic_cast<relObject*>(i_pObject);
	if (pRelObject)
		m_ObjectRef = pRelObject->CreateReferenceToSelf();
	else
		DBG_WARNING("Could not create reference in material set undo operation.");
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string mtrlSetOperation::GetDisplayName()
{
	return m_DisplayName.c_str();
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float mtrlSetOperation::GetMemoryUsage()
{
	return (sizeof(mdlMaterialInfo) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void mtrlSetOperation::Undo()
{
	if (m_ObjectRef)
	{
		relObject *pObject = m_ObjectRef->GetObject();
		if (pObject)
		{
			mtrlScriptObject *pMatObject = dynamic_cast<mtrlScriptObject*>(pObject);
			if (pMatObject)
			{
				mdlMaterialInfo temp = pMatObject->GetMaterialData(m_MaterialIndex);
				pMatObject->ChangeMaterialData(m_MaterialIndex, m_DataBackup, m_bUpdateProperties);
				m_DataBackup = temp; // switch backup from undo to redo
				cmmObjectDialogUtil::UpdateDialog();
			}
			else
				DBG_WARNING("Material set undo: Resolved object is not a material type.");
		}
		else
			DBG_WARNING("Could not resolve reference in material set undo operation.");
	}
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void mtrlSetOperation::Redo()
{
	if (m_ObjectRef)
	{
		relObject *pObject = m_ObjectRef->GetObject();
		if (pObject)
		{
			mtrlScriptObject *pMatObject = dynamic_cast<mtrlScriptObject*>(pObject);
			if (pMatObject)
			{
				mdlMaterialInfo temp = pMatObject->GetMaterialData(m_MaterialIndex);
				pMatObject->ChangeMaterialData(m_MaterialIndex, m_DataBackup, m_bUpdateProperties);
				m_DataBackup = temp; // switch backup from undo to redo
				cmmObjectDialogUtil::UpdateDialog();
			}
			else
				DBG_WARNING("Material set redo: Resolved object is not a material type.");
		}
		else
			DBG_WARNING("Could not resolve reference in material set redo operation.");
	}
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void mtrlSetOperation::Commit()
{
	// nothing needed
}

//--------------------------------------------------------------------
//  Destroy is called on an operation when it has been undone
// and it can no longer be redone. This may happen after the
// history gets long enough or a new operation is made when
// its current state is "undone". The destructor will soon be
// called.
//--------------------------------------------------------------------
void mtrlSetOperation::Destroy()
{
	// nothing needed
}
