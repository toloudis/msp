/********************************************************************************************\
**  orthoCMSRequestTask.hpp
**
**		Data for the CMSRequests
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef ORTHO_CMSREQUESTTASK_HPP
#error orthoCMSRequestTask.hpp multiply included
#endif
#define ORTHO_CMSREQUESTTASK_HPP

#ifndef ORTHO_REQUESTTASK_HPP
#include "Features/Requests/Tasks/orthoRequestTask.hpp"
#endif

#include <string>


//============================================================================
//	Task: SaveXMLScene
//============================================================================
class orthoCMSRequestSaveXMLSceneTask : public orthoRequestTask
{
public:
	orthoCMSRequestSaveXMLSceneTask() : orthoRequestTask("savexmlscene", SAVEXMLSCENE_ID ) {};

	virtual void ExecuteTask();

public:
	//std::string	m_FileName;
};


//============================================================================
//	Task: SaveScene
//============================================================================
class orthoCMSRequestSaveSceneTask : public orthoRequestTask
{
public:
	orthoCMSRequestSaveSceneTask() : orthoRequestTask("savescene", SAVESCENE_ID ) {};

	virtual void ExecuteTask();

public:
	//std::string	m_FileName;
};


//============================================================================
//	Task: SaveXMLCharacter
//============================================================================
class orthoCMSRequestSaveXMLCharacterTask : public orthoRequestTask
{
public:
	orthoCMSRequestSaveXMLCharacterTask() : orthoRequestTask("savexmlcharacter", SAVEXMLCHARACTER_ID ) {};

	virtual void ExecuteTask();

	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }

public:
	std::string	m_FileName;
};
