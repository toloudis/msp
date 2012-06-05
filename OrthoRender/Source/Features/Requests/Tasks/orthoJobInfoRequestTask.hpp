/********************************************************************************************\
**  orthoJobInfoRequestTask.hpp
**
**		Data for the JobInfoRequests
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\********************************************************************************************/
#ifdef ORTHO_JOBINFOREQUESTTASK_HPP
#error orthoJobInfoRequestTask.hpp multiply included
#endif
#define ORTHO_JOBINFOREQUESTTASK_HPP

#ifndef ORTHO_REQUESTTASK_HPP
#include "Features/Requests/Tasks/orthoRequestTask.hpp"
#endif

#include <string>


//============================================================================
//	Task: JobInfo
//============================================================================
class orthoJobInfoRequestTask : public orthoRequestTask
{
public:
	orthoJobInfoRequestTask() : orthoRequestTask("jobinfo", JOBINFO_ID ) {};

	virtual void ExecuteTask();

	virtual void SetJobName( const std::string& i_Name ){ m_JobName = i_Name; }

public:
	std::string	m_JobName;
};
