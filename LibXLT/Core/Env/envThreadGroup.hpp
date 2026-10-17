/****************************************************************************\
**  envThreadGroup.hpp
**
**      envThreadGroup.hpp provides a way to run a group of tasks
**	concurrently and then wait for them to finish.
**	If single-threaded, the tasks will complete in order.
**	If multithreaded, the tasks run on a shared pool of worker threads
**	that is created on first use and reused, so adding a task doesn't
**	create a thread.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef ENV_THREADGROUP_HPP
#error envThreadGroup.hpp multiply included
#endif
#define ENV_THREADGROUP_HPP

#ifndef ENV_THREAD_HPP
#include "Core/env/envThread.hpp"
#endif

#include <functional>
#include <memory>

//========================================================================
// envThreadGroup is a manager for a set of tasks that can be
//	run concurrently.
//========================================================================
class envThreadGroup
{
public:
	typedef std::function<void()> ThreadFunc;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void SetThreadingEnabled(bool i_bEnabled);
	static bool GetThreadingEnabled();

	//--------------------------------------------------------------------
	// Stop and join the shared worker threads. Called by
	//	envPackage::CleanUp(); the workers are recreated if a group
	//	is used again afterwards.
	//--------------------------------------------------------------------
	static void ShutDownWorkers();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	envThreadGroup();
	~envThreadGroup();

	envThreadGroup(const envThreadGroup&) = delete;
	envThreadGroup& operator=(const envThreadGroup&) = delete;

	//--------------------------------------------------------------------
	// Add a task to the group to be managed. The argument could be
	//	a function pointer or a function object.
	//--------------------------------------------------------------------
	void AddThread(const ThreadFunc& i_ThreadFunc);

	//--------------------------------------------------------------------
	// Wait for all tasks to finish. While waiting, the calling thread
	//	runs queued tasks itself. If a task threw an exception, the
	//	first one is rethrown here once all tasks have finished.
	//--------------------------------------------------------------------
	void WaitForAll();

	struct Batch;

private:
	std::unique_ptr<Batch> m_pBatch;
	static bool sm_bEnableThreading;
};

