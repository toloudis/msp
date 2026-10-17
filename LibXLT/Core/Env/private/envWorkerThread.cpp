/****************************************************************************\
**  envWorkerThread.cpp
**
**		see .hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Core/env/envWorkerThread.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
envWorkerThread::envWorkerThread()
:	m_bStop(false)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envWorkerThread::~envWorkerThread()
{
	{
		std::scoped_lock lock(m_Mutex);
		m_bStop = true;
	}
	m_JobAvailable.notify_one();
	if (m_Thread.joinable())
		m_Thread.join();
}

//--------------------------------------------------------------------
// Queue a job to run on the thread and return without waiting.
//--------------------------------------------------------------------
void envWorkerThread::Post(const Job& i_Job)
{
	{
		std::scoped_lock lock(m_Mutex);
		m_Jobs.push_back(i_Job);
		if (!m_Thread.joinable())
			m_Thread = std::thread([this]() { thread_loop(); });
	}
	m_JobAvailable.notify_one();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envWorkerThread::thread_loop()
{
	for (;;)
	{
		Job job;
		{
			std::unique_lock lock(m_Mutex);
			m_JobAvailable.wait(lock, [this]() { return m_bStop || !m_Jobs.empty(); });
			if (m_Jobs.empty())
				return;		// stopping, and nothing left to run
			job = std::move(m_Jobs.front());
			m_Jobs.pop_front();
		}
		job();
	}
}

