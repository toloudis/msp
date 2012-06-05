/*****************************************************************************
**	chnlDeleteDriverOperation.cpp
**
**	 Undo operation for deleting drivers
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlDeleteDriverOperation.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"


//============================================================================
//============================================================================
namespace
{
	//============================================================================
	//============================================================================
	const char *c_DeleteOperationDisplayName = "Delete Driver";
}

//--------------------------------------------------------------------
// constructor takes old to be restored if undone
// takes ownership of the DriverInfo, but not the script object.
//--------------------------------------------------------------------
chnlDeleteDriverOperation::chnlDeleteDriverOperation(shared_ptr<relObjectReference> i_ScriptObjectRef,
							  tmlnDriverInfo* i_pInfo,
							  int i_DriverIndex)
:	m_ScriptObjectRef(i_ScriptObjectRef), 
	m_pInfo(i_pInfo), 
	m_pDriver(NULL),
	m_DriverIndex(i_DriverIndex)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlDeleteDriverOperation::~chnlDeleteDriverOperation()
{
	// Only the driver info is owned by this class.
	// If our m_pDriver pointer is non-NULL it means that
	// it is still being used by the script object.
	delete m_pInfo;
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string  chnlDeleteDriverOperation::GetDisplayName()
{
	//char displaytext[128];
	//sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, m_pInfo->m_Name.c_str());
	std::ostringstream oss;
	oss << c_DeleteOperationDisplayName <<" - "<<m_pInfo->m_Name;
	std::string displaytext(oss.str());

	return displaytext;
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float  chnlDeleteDriverOperation::GetMemoryUsage()
{
	// really needs size of derived data
	return (sizeof(tmlnDriverInfo) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void  chnlDeleteDriverOperation::Undo()
{
	relObject *pObject = m_ScriptObjectRef->GetObject();
	if (pObject)
	{
		tmlnScriptObject *pScriptObject = dynamic_cast<tmlnScriptObject*>(pObject);
		if (pScriptObject)
		{
			m_pDriver = tmlnCreator::RestoreDriverFromInfo(pScriptObject, m_pInfo, m_DriverIndex);
			chnlDialogUtil::UpdateChannels();
		}
		else
			DBG_WARNING("Delete driver undo: Resolved object is not a script object type.");
	}
	else
		DBG_WARNING("Could not resolve reference in delete driver undo operation.");
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void  chnlDeleteDriverOperation::Redo()
{
	relObject *pObject = m_ScriptObjectRef->GetObject();
	if (pObject)
	{
		tmlnScriptObject *pScriptObject = dynamic_cast<tmlnScriptObject*>(pObject);
		if (pScriptObject)
		{
			if (pScriptObject->RemoveDriver(m_pDriver, m_DriverIndex))
			{
				delete m_pDriver;
				m_pDriver = NULL;
				chnlDialogUtil::UpdateChannels();
			}
		}
		else
			DBG_WARNING("Delete driver redo: Resolved object is not a script object type.");
	}
	else
		DBG_WARNING("Could not resolve reference in delete driver redo operation.");
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void  chnlDeleteDriverOperation::Commit()
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
void  chnlDeleteDriverOperation::Destroy()
{
	// nothing needed
}

