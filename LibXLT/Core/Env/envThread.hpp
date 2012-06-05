/****************************************************************************\
**  envThread.hpp
**
**      envThread.hpp wraps the boost thread classes.
**	It gives you one place to turn on and off multihthreading.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef ENV_THREAD_HPP
#error envThread.hpp multiply included
#endif
#define ENV_THREAD_HPP

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

// Use this define to control whether to use threading
// Only have boost threads compiled for VC++9 in 64-bit
#if _MSC_VER >= 1500 
#define ENV_USE_THREADS 1
#endif


#ifdef ENV_USE_THREADS

	#define BOOST_THREAD_USE_DLL 1

	#include <boost/thread/thread.hpp>
	#include <boost/thread/mutex.hpp>

	typedef boost::thread envThread;
	typedef boost::mutex envMutex;
	typedef boost::mutex::scoped_lock envScopedLock;

#else

	#include <boost/function.hpp>

	//========================================================================
	//	The single threaded version of envThread just executes
	//	it function immediately.
	//========================================================================
	class envThread
	{
	public:
		envThread() {}
		explicit envThread(const boost::function0<void>& i_ThreadFunc)
		{
			// Execute function immediately
			i_ThreadFunc();
		}

		void join() {}

		// This function returns the number of processors on the system.
		// If threading is disabled, then return 0
		static unsigned hardware_concurrency()
		{
			return 0;
		}

	};

	//========================================================================
	//	The single threaded version of envMutex doesn't do anything
	//========================================================================
	class envMutex
	{

	};

	//========================================================================
	//	The single threaded version of envMutex doesn't do anything
	//========================================================================
	class envScopedLock
	{
	public:
		explicit envScopedLock(envMutex& i_Mutex) {}

	};


#endif
