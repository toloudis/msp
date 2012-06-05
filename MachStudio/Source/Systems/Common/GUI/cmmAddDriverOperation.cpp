/*****************************************************************************
**	cmmAddDriverOperation.cpp
**
**	 Undo operation for adding drivers
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/cmmAddDriverOperation.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"


//============================================================================
//============================================================================
namespace
{
	const char *c_AddOperationDisplayName = "Add Driver";
}


//--------------------------------------------------------------------
// constructor takes old to be restored if undone
//--------------------------------------------------------------------
cmmAddDriverOperation::cmmAddDriverOperation(shared_ptr<relObjectReference> i_ScriptObjectRef,
											 tmlnDriver* i_pDriver)
:	m_ScriptObjectRef(i_ScriptObjectRef), 
	m_pDriver(i_pDriver), 
	m_pInfo(NULL),
	m_DriverIndex(0)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmAddDriverOperation::~cmmAddDriverOperation()
{
	// Only the driver info is owned by this class, and only if it is non-NULL
	if (m_pInfo)
	{
		delete m_pInfo;
	}
}

//--------------------------------------------------------------------
//  Get Name for the operation
//--------------------------------------------------------------------
std::string  cmmAddDriverOperation::GetDisplayName()
{
	//char displaytext[256];
	//sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, 
	//	m_pInfo ? m_pInfo->m_Name.c_str() : m_pDriver->GetName().c_str());

	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss << c_AddOperationDisplayName << " - " << m_pInfo ? m_pInfo->m_Name.c_str() : m_pDriver->GetName().c_str();
	std::string displaytext(oss.str());
	return displaytext;
}

//--------------------------------------------------------------------
//  Get memory usage for this operation (in KB). This can be
// accurate or approximate.
//--------------------------------------------------------------------
float  cmmAddDriverOperation::GetMemoryUsage()
{
	// really needs size of derived data
	return (sizeof(tmlnDriverInfo) / 1000.0f); // convert to KB
}

//--------------------------------------------------------------------
//  Undo is called on an operation when the user chooses
// Edit->Undo from the menu.
//--------------------------------------------------------------------
void  cmmAddDriverOperation::Undo()
{
	// Save info in order to restore the driver
	if (!m_pInfo)
		m_pInfo = m_pDriver->GetDriverInfo();

	relObject *pObject = m_ScriptObjectRef->GetObject();
	if (pObject)
	{
		tmlnScriptObject *pScriptObject = dynamic_cast<tmlnScriptObject*>(pObject);
		if (pScriptObject)
		{
			// Disconnect and delete driver
			if (pScriptObject->RemoveDriver(m_pDriver, m_DriverIndex))
			{
				delete m_pDriver;
				m_pDriver = NULL;
				chnlDialogUtil::UpdateChannels();
			}
		}
		else
			DBG_WARNING("Add driver undo: Resolved object is not a script object type.");
	}
	else
		DBG_WARNING("Could not resolve reference in add driver undo operation.");

}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void  cmmAddDriverOperation::Redo()
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
			DBG_WARNING("Add driver redo: Resolved object is not a script object type.");
	}
	else
		DBG_WARNING("Could not resolve reference in add driver redo operation.");
}

//--------------------------------------------------------------------
//  Commit is called on an operation when it is no longer
// possible for the user to undo this operation.  The
// destructor will soon be called.
//--------------------------------------------------------------------
void  cmmAddDriverOperation::Commit()
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
void  cmmAddDriverOperation::Destroy()
{
	// nothing needed
}

