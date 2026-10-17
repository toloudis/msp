/*****************************************************************************
**  envWorkerThreadTest.cpp
**
**      envWorkerThread runs posted jobs in order on one thread that
**      isn't the caller's, and its destructor waits for them.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef ENV_WORKERTHREAD_HPP
#include "Core/env/envWorkerThread.hpp"
#endif

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <set>
#include <vector>

TEST(envWorkerThreadTest, RunsJobsInOrderOnOneOtherThread)
{
	std::vector<int> order;
	std::set<std::thread::id> ids;
	{
		envWorkerThread worker;
		for( int i = 0 ; i < 100 ; i++ )
		{
			worker.Post([&order, &ids, i]()
			{
				order.push_back(i);
				ids.insert(std::this_thread::get_id());
			});
		}
	}	// waits for the jobs

	ASSERT_EQ(100u, order.size());
	for( int i = 0 ; i < 100 ; i++ )
		EXPECT_EQ(i, order[i]);
	ASSERT_EQ(1u, ids.size());
	EXPECT_NE(std::this_thread::get_id(), *ids.begin());
}

TEST(envWorkerThreadTest, PostReturnsWithoutWaiting)
{
	std::atomic<bool> release(false);
	std::atomic<bool> finished(false);
	{
		envWorkerThread worker;
		worker.Post([&]()
		{
			while (!release)
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			finished = true;
		});

		// Post returned while the job is still blocked
		EXPECT_FALSE(finished.load());
		release = true;
	}
	EXPECT_TRUE(finished.load());
}

TEST(envWorkerThreadTest, DestroyingWithoutJobsIsFine)
{
	envWorkerThread worker;
}

