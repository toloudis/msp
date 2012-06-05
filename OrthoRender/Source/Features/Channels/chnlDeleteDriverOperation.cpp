/*****************************************************************************
**	chnlDeleteDriverOperation.cpp
**
**	 Undo operation for deleting drivers
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/chnlDeleteDriverOperation.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"

#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"


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
chnlDeleteDriverOperation::chnlDeleteDriverOperation(tmlnScriptObject* i_pObject,
							  tmlnDriverInfo* i_pInfo)
:	m_pObject(i_pObject), 
	m_pInfo(i_pInfo), 
	m_pDriver(NULL)
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
	char displaytext[128];
	sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, m_pInfo->m_Name.c_str());
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
	m_pDriver = tmlnCreator::CreateDriverFromInfo(m_pObject, m_pInfo);
	chnlDialogUtil::UpdateChannels();
}

//--------------------------------------------------------------------
// Redo is called on an operation when the user chooses
// Edit->Redo from the menu and this operation is the next in
// line to be redone.
//--------------------------------------------------------------------
void  chnlDeleteDriverOperation::Redo()
{
	if (m_pObject->RemoveDriver(m_pDriver))
	{
		delete m_pDriver;
		m_pDriver = NULL;
		chnlDialogUtil::UpdateChannels();
	}
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

