/********************************************************************************************\
**  orthoCMSRequestTask.cpp
**
**		Data for the CMSRequests
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/

#include "Features/Requests/Tasks/orthoCMSRequestTask.hpp"
#include "Features/Requests/orthoRemoteCommandMgr.hpp"
#include "Features/Capture/orthoAvatarDataUtil.hpp"

#include "Core/App/appTime.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"


//--------------------------------------------------------------------
//	orthoCMSRequestSaveXMLSceneTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoCMSRequestSaveXMLSceneTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "savexmlscene", "Bad orthoCMSRequestSaveXMLSceneTask name" );
	float start_time = appTime::GetTime();

	docSingleDocumentMgr::SetWriteFormat( docDocument::eDocXML );
	docSingleDocumentMgr::SaveDocument();

	float end_time = appTime::GetTime();
	DBG_WARNING2("CMS Save XML (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));
}

//--------------------------------------------------------------------
//	orthoCMSRequestSaveSceneTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoCMSRequestSaveSceneTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "savescene", "Bad orthoCMSRequestSaveSceneTask name" );

	float start_time = appTime::GetTime();

	fsLocator newfname = docSingleDocumentMgr::GetFilename();
	newfname.ReplaceLastName(itString("Test.mab"));
	docSingleDocumentMgr::SetFilename( newfname );
	docSingleDocumentMgr::SetWriteFormat( docDocument::eDocBinary );
	docSingleDocumentMgr::SaveDocument();

	float end_time = appTime::GetTime();
	DBG_WARNING2("CMS Save Scene (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));
}

//--------------------------------------------------------------------
//	orthoCMSRequestSaveXMLCharacterTask ExecuteTask - return true if task handled
//--------------------------------------------------------------------
void orthoCMSRequestSaveXMLCharacterTask::ExecuteTask()
{
	DBG_ASSERT0( GetTaskName() == "savexmlcharacter", "Bad orthoCMSRequestSaveXMLCharacterTask name" );

	float start_time = appTime::GetTime();

	DBG_LOG1(" Finding character (%s) to write as XML", m_FileName.c_str());
	orthoAvatarDataUtil::SaveXMLCharacter( m_FileName );

	float end_time = appTime::GetTime();
	DBG_WARNING2("CMS Save XML Character (%s) Execute took %6.3f seconds", GetTaskName().c_str(), (end_time - start_time));
}