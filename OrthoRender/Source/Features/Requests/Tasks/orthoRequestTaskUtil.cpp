/*****************************************************************************
**	orthoRequestTaskUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include <windows.h>
#include "Features/Requests/Tasks/orthoRequestTaskUtil.hpp"

#include "Core/Dbg/dbgLog.hpp"
#include "Core/Env/envSTLHelpers.hpp"

#ifndef ORTHO_XMLCOMMANDPARSER_HPP
#include "Features/Requests/orthoXMLCommandParser.hpp"
#endif

#include "Core/App/appTime.hpp"
#include "Features/Requests/orthoRemoteCommandMgr.hpp"

#include <string>
#include <vector>
#include <algorithm>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool orthoRequestTask::SortLess( const orthoRequestTask* first, const orthoRequestTask* second )
{
	return first->m_TaskID < second->m_TaskID;
}


//============================================================================
//============================================================================
namespace orthoRequestTaskUtil
{
	orthoRequestJobs m_Jobs;		//list of jobs
	orthoXMLCommandParser* m_Parser = NULL;

	CRITICAL_SECTION l_CriticalTask;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Initialize()
	{
		InitializeCriticalSection( &l_CriticalTask );

		orthoRequestTaskUtil::m_Jobs.clear();
		orthoRequestTaskUtil::m_Jobs.resize(0);

		DBG_ASSERT0( m_Parser == NULL, "Invalid Parser, already initialized!" );
		m_Parser = new orthoXMLCommandParser;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeInitialize()
	{
		DeleteJobs();
		orthoRequestTaskUtil::m_Jobs.resize(0);

		delete m_Parser;
		m_Parser = NULL;

		DeleteCriticalSection( &l_CriticalTask );
	}

	//--------------------------------------------------------------------
	//	ParseXMLBlock - returns true if block was parsed.
	//--------------------------------------------------------------------
	bool ParseXMLCommandString(	std::string& io_XMLString )
	{
		if( m_Parser )
		{
			m_Parser->Clear();
			return m_Parser->ParseBlock( io_XMLString );
		}
		return false;
	}
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	orthoRequestJobs& GetJobs()
	{
		return m_Jobs;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	orthoRequestJob* GetCurrentJob()
	{
		return orthoRequestTaskUtil::m_Jobs.back();
	}

	//------------------------------------------------------------------------
	//	Start a new job
	//------------------------------------------------------------------------
	void StartNewJob()
	{
		EnterCriticalSection( &l_CriticalTask );
		orthoRequestTaskUtil::m_Jobs.push_back( new orthoRequestJob() );
		LeaveCriticalSection( &l_CriticalTask );
	}

	//------------------------------------------------------------------------
	//	this now becomes the owner of this task
	//------------------------------------------------------------------------
	void AddTaskToCurrentJob(orthoRequestTask* i_pTask)
	{
		if (i_pTask != NULL)
		{
			EnterCriticalSection( &l_CriticalTask );
			GetCurrentJob()->push_back( i_pTask );
			LeaveCriticalSection( &l_CriticalTask );
		}
	}

	//------------------------------------------------------------------------
	//	ExecuteJobs
	//------------------------------------------------------------------------
	bool ExecuteJobs()
	{
		float start_time = appTime::GetTime();

//		EnterCriticalSection( &l_CriticalTask );
		bool bSceneChanged = false;
		int size = orthoRequestTaskUtil::m_Jobs.size();
		for (int i=0; i < size; ++i)
		{
			orthoRequestJob& job = *orthoRequestTaskUtil::m_Jobs[i];

			//sort the tasks by there Task ID, lower first
			std::sort( job.begin(), job.end(), orthoRequestTask::SortLess );

			//Execute each task, note virtual function will call the proper method.
			std::for_each( job.begin(), job.end(), std::mem_fun( &orthoRequestTask::ExecuteTask) );
			bSceneChanged = true;
		}

		DeleteJobs();
//		LeaveCriticalSection( &l_CriticalTask );

		float end_time = appTime::GetTime();
		if( bSceneChanged )
		{
			DBG_LOG("Executing Tasks took " << (end_time - start_time) << " seconds");

			orthoRemoteCommandMgr::RemoteRenderData& rr = orthoRemoteCommandMgr::m_RenderParameters[0];
			if (rr.GetRenderResponse() != NULL) 
			{
				rr.GetRenderResponse()->setCommandProcessingTime((end_time - start_time)) ;
			}
		}

		return bSceneChanged;
	}

	//------------------------------------------------------------------------
	//	Delete the jobs
	//------------------------------------------------------------------------
	void DeleteJobs()
	{
		orthoRequestJobs::iterator it = orthoRequestTaskUtil::m_Jobs.begin();
		while( it != orthoRequestTaskUtil::m_Jobs.end() )
		{
//			orthoRequestJob& job = *(*it);
			envSTLHelpers::DeleteContainer( **it );
			it++;
		}
		envSTLHelpers::DeleteContainer( orthoRequestTaskUtil::m_Jobs );
	}

	//------------------------------------------------------------------------
	//	DisplayJobs - debug output for the jobs list
	//------------------------------------------------------------------------
	void DisplayJobs()
	{
		int size = m_Jobs.size();
		for (int i=0; i < size; ++i)
		{
			DBG_LOG(i << " Jobs");
			orthoRequestJob& job = *m_Jobs[i];

			int tsize = job.size();
			DBG_LOG("    " << tsize << " Tasks");
			for (int j=0; j < tsize; ++j)
			{
				DBG_LOG("        " << j << ") " << job[j]->GetTaskName().c_str() );
			}
		}
	}
}

