/********************************************************************************************\
**  orthoObjectsRequestTask.hpp
**
**		Data for the ObjectsRequests
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef ORTHO_OBJECTSREQUESTTASK_HPP
#error orthoObjectsRequestTask.hpp multiply included
#endif
#define ORTHO_OBJECTSREQUESTTASK_HPP

#ifndef LIGHTWAIT_RESPONSE_HPP
#include "MainApp/LightWaitResponse.hpp"
#endif
#ifndef ORTHO_REQUESTTASK_HPP
#include "Features/Requests/Tasks/orthoRequestTask.hpp"
#endif

#include <string>


//============================================================================
//	Load a model
//============================================================================
class orthoObjectsRequestModelTask : public orthoRequestTask
{
public:
	orthoObjectsRequestModelTask() 
	:	orthoRequestTask("model", MODEL_ID ),
		m_bIsBaseModel(false)
	{};

	virtual void ExecuteTask();

	virtual void SetIsBaseModel( bool val ){ m_bIsBaseModel = val; }
	virtual void SetName( const std::string& i_Name ){ m_Name = i_Name; }
	virtual void SetFileName( const std::string& i_FileName ){ m_FileName = i_FileName; }

public:
	bool m_bIsBaseModel;
	std::string	m_Name;
	std::string	m_FileName;
};


//============================================================================
//	Load a series of models
//============================================================================
class orthoObjectsRequestModelsTask : public orthoRequestTask
{
public:
	orthoObjectsRequestModelsTask() : orthoRequestTask("models", MODELS_ID ) {};

	virtual void ExecuteTask();

public:
	std::vector<orthoObjectsRequestModelTask> m_Tasks;
};

