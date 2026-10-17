/****************************************************************************\
**  envWorkerThread.hpp
**
**      envWorkerThread is one long-lived thread that runs posted jobs
**	in order, for work that is handed to another thread over and over
**	(such as rendering a frame) without creating a thread each time.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef ENV_WORKERTHREAD_HPP
#error envWorkerThread.hpp multiply included
#endif
#define ENV_WORKERTHREAD_HPP

#ifndef ENV_THREAD_HPP
#include "Core/env/envThread.hpp"
#endif

#include <condition_variable>
#include <deque>
#include <functional>

//========================================================================
//========================================================================
class envWorkerThread
{
public:
	typedef std::function<void()> Job;

	//--------------------------------------------------------------------
	// The thread starts with the first Post(). The destructor waits for
	//	the posted jobs to finish, then stops the thread.
	//--------------------------------------------------------------------
	envWorkerThread();
	~envWorkerThread();

	envWorkerThread(const envWorkerThread&) = delete;
	envWorkerThread& operator=(const envWorkerThread&) = delete;

	//--------------------------------------------------------------------
	// Queue a job to run on the thread and return without waiting.
	//	A job that throws terminates the program, as an exception
	//	escaping a std::thread does.
	//--------------------------------------------------------------------
	void Post(const Job& i_Job);

private:
	void thread_loop();

	std::thread m_Thread;
	std::deque<Job> m_Jobs;
	std::mutex m_Mutex;
	std::condition_variable m_JobAvailable;
	bool m_bStop;
};

