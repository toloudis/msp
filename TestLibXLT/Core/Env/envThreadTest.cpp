/*****************************************************************************
**  envThreadTest.cpp
**
**      envThreadTest is a test of envThread.hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "envThreadTest.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envThread.hpp"
#include "Core/env/envThreadGroup.hpp"


//====================================================================
// Anonymous Namespace for local variables and functions
//====================================================================
namespace 
{
	void hello1()
	{
		for (int i=0; i<6; i++)
		{
			DBG_LOG0("I say hello, world.");
		}
	}
	void hello2()
	{
		DBG_LOG0("Hello world, I'm a thread!");
	}

	std::string l_Buffer;
	envMutex l_BufferMutex;

	void lower_case_fill()
	{
		for (char ch='a'; ch <= 'd'; ch++)
		{
			envScopedLock lock(l_BufferMutex);
			for (int i=0; i<5; i++)
			{
				l_Buffer += ch;

				// Loops to slow things down
				for (int j=0; j<10000; j++)
				{
					char ch1 = 'J';
				}
			}
		}

	}
	void upper_case_fill()
	{
		for (char ch='A'; ch <= 'D'; ch++)
		{
			envScopedLock lock(l_BufferMutex);
			for (int i=0; i<5; i++)
				l_Buffer += ch;
		}
	}

	
	void count_really_high()
	{
		for (int j=0; j<10000; j++)
		{
			// Waste a little time
			char *pChar = new char;
			delete pChar;
		}
		DBG_LOG0("Done counting.");
	}
}

//========================================================================
//	RunTest - executes the envSTLHelpers tests
//========================================================================
void envThreadTest::RunTest()
{
	DBG_LOG0("Begin envThreadTest::RunTest()");

	//Note: Try changing the value of ENV_USE_THREADS in envThread.hpp
	// and see how the output of this test program changes.
#ifdef ENV_USE_THREADS
	DBG_LOG0("Multi-threaded");
#else
	DBG_LOG0("Single-threaded");
#endif

	envThread thrd1(&hello1);
	envThread thrd2(&hello2);
	thrd1.join();
	thrd2.join();

	DBG_LOG0("Begin mutex test");
	envThread thrd3(&lower_case_fill);
	envThread thrd4(&upper_case_fill);
	thrd3.join();
	thrd4.join();
	DBG_LOG1("Fill results: %s", l_Buffer.c_str());
	DBG_LOG0("End mutex test");

	DBG_LOG0("Begin thread group test");
	envThreadGroup threads;
	threads.AddThread(&count_really_high);
	threads.AddThread(&count_really_high);
	DBG_LOG0("Waiting...");
	threads.WaitForAll();
	DBG_LOG0("End thread group test");

	DBG_LOG0("\nEnd envThreadTest::RunTest()");
}
