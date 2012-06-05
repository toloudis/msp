/********************************************************************************************\
**  orthoSceneRequestTask.hpp
**
**		Data for the SceneRequests
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef ORTHO_SCENEREQUESTTASK_HPP
#error orthoSceneRequestTask.hpp multiply included
#endif
#define ORTHO_SCENEREQUESTTASK_HPP

#ifndef ORTHO_REQUESTTASK_HPP
#include "Features/Requests/Tasks/orthoRequestTask.hpp"
#endif

#include <string>


//============================================================================
//	Task: Scene
//============================================================================
class orthoSceneRequestLoadTask : public orthoRequestTask
{
public:
	orthoSceneRequestLoadTask() : orthoRequestTask("scene", SCENE_ID ) {};

	virtual void ExecuteTask();

	virtual void SetFileName( const std::string& i_Name ){ m_FileName = i_Name; }

public:
	std::string	m_FileName;
};
