/****************************************************************************\
**  envThreadGroup.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "Core/env/envThreadGroup.hpp"

#include <algorithm>
#include <condition_variable>
#include <deque>
#include <exception>
#include <mutex>
#include <vector>


//====================================================================
// static member variables
//====================================================================
bool envThreadGroup::sm_bEnableThreading = false;


//====================================================================
// The tasks of one group still to finish, and the first exception
//	one of them threw.
//====================================================================
struct envThreadGroup::Batch
{
	std::mutex m_Mutex;
	std::condition_variable m_Finished;
	int m_nPending = 0;
	std::exception_ptr m_pError;
};

#ifdef ENV_USE_THREADS
//====================================================================
//====================================================================
namespace
{
	struct sTask
	{
		envThreadGroup::Batch* m_pBatch;
		envThreadGroup::ThreadFunc m_Func;
	};

	//----------------------------------------------------------------
	// Run a task and mark it finished in its batch. The batch may be
	// destroyed as soon as the mutex is released, so that is the
	// last access to it.
	//----------------------------------------------------------------
	void run_task(sTask& i_Task)
	{
		std::exception_ptr error;
		try
		{
			i_Task.m_Func();
		}
		catch (...)
		{
			error = std::current_exception();
		}

		envThreadGroup::Batch& batch = *i_Task.m_pBatch;
		std::scoped_lock lock(batch.m_Mutex);
		if (error && !batch.m_pError)
			batch.m_pError = error;
		if (--batch.m_nPending == 0)
			batch.m_Finished.notify_all();
	}

	//================================================================
	// Worker threads shared by all groups, taking tasks from one queue
	//================================================================
	class cWorkerPool
	{
	public:
		explicit cWorkerPool(unsigned i_nWorkers)
		{
			for (unsigned i = 0; i < i_nWorkers; i++)
				m_Workers.emplace_back([this]() { worker_loop(); });
		}

		~cWorkerPool()
		{
			{
				std::scoped_lock lock(m_Mutex);
				m_bStop = true;
			}
			m_WorkAvailable.notify_all();
			for (std::thread& worker : m_Workers)
				worker.join();
		}

		void Push(sTask&& i_Task)
		{
			{
				std::scoped_lock lock(m_Mutex);
				m_Queue.push_back(std::move(i_Task));
			}
			m_WorkAvailable.notify_one();
		}

		bool TryPop(sTask& o_Task)
		{
			std::scoped_lock lock(m_Mutex);
			if (m_Queue.empty())
				return false;
			o_Task = std::move(m_Queue.front());
			m_Queue.pop_front();
			return true;
		}

	private:
		void worker_loop()
		{
			for (;;)
			{
				sTask task;
				{
					std::unique_lock lock(m_Mutex);
					m_WorkAvailable.wait(lock, [this]() { return m_bStop || !m_Queue.empty(); });
					if (m_Queue.empty())
						return;		// stopping, and nothing left to run
					task = std::move(m_Queue.front());
					m_Queue.pop_front();
				}
				run_task(task);
			}
		}

		std::vector<std::thread> m_Workers;
		std::deque<sTask> m_Queue;
		std::mutex m_Mutex;
		std::condition_variable m_WorkAvailable;
		bool m_bStop = false;
	};

	std::mutex l_PoolMutex;
	std::unique_ptr<cWorkerPool> l_pPool;

	//----------------------------------------------------------------
	// The thread waiting in WaitForAll() also runs tasks, so one
	// fewer worker than processors keeps them all busy.
	//----------------------------------------------------------------
	cWorkerPool& get_pool()
	{
		std::scoped_lock lock(l_PoolMutex);
		if (!l_pPool)
		{
			unsigned processors = std::thread::hardware_concurrency();
			l_pPool = std::make_unique<cWorkerPool>((std::max)(processors, 2u) - 1);
		}
		return *l_pPool;
	}
}
#endif


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void envThreadGroup::SetThreadingEnabled(bool i_bEnabled)
{
	envThreadGroup::sm_bEnableThreading = i_bEnabled;
}
bool envThreadGroup::GetThreadingEnabled()
{
	return envThreadGroup::sm_bEnableThreading;
}

//--------------------------------------------------------------------
// Stop and join the shared worker threads.
//--------------------------------------------------------------------
void envThreadGroup::ShutDownWorkers()
{
#ifdef ENV_USE_THREADS
	std::unique_ptr<cWorkerPool> pool;
	{
		std::scoped_lock lock(l_PoolMutex);
		pool = std::move(l_pPool);
	}
	pool.reset();	// joins, outside the lock
#endif
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
envThreadGroup::envThreadGroup()
:	m_pBatch(std::make_unique<Batch>())
{
}

// The tasks refer to m_pBatch, so they must finish first
envThreadGroup::~envThreadGroup()
{
	try
	{
		WaitForAll();
	}
	catch (...)
	{
		// a task's exception can't be reported from a destructor
	}
}

//--------------------------------------------------------------------
// Add a task to the group to be managed. The argument could be
//	a function pointer or a function object.
//--------------------------------------------------------------------
void envThreadGroup::AddThread(const ThreadFunc& i_ThreadFunc)
{
#ifdef ENV_USE_THREADS
	if (envThreadGroup::sm_bEnableThreading)
	{
		{
			std::scoped_lock lock(m_pBatch->m_Mutex);
			m_pBatch->m_nPending++;
		}
		get_pool().Push(sTask{ m_pBatch.get(), i_ThreadFunc });
	}
	else
		i_ThreadFunc();
#else
	// If not using threads, execute function immediately
	i_ThreadFunc();
#endif
}

//--------------------------------------------------------------------
// Wait for all tasks to finish.
//--------------------------------------------------------------------
void envThreadGroup::WaitForAll()
{
#ifdef ENV_USE_THREADS
	Batch& batch = *m_pBatch;
	for (;;)
	{
		{
			std::scoped_lock lock(batch.m_Mutex);
			if (batch.m_nPending == 0)
				break;
		}

		// Help with queued tasks (this group's or another's) rather than
		// block; this also keeps a task that waits on a group of its own
		// from deadlocking when every worker is busy.
		sTask task;
		if (get_pool().TryPop(task))
		{
			run_task(task);
		}
		else
		{
			// The rest of our tasks are running on workers
			std::unique_lock lock(batch.m_Mutex);
			batch.m_Finished.wait(lock, [&batch]() { return batch.m_nPending == 0; });
			break;
		}
	}

	std::exception_ptr error;
	{
		std::scoped_lock lock(batch.m_Mutex);
		std::swap(error, batch.m_pError);
	}
	if (error)
		std::rethrow_exception(error);
#endif
}

