/*****************************************************************************
**  envThreadGroupTest.cpp
**
**      envThreadGroup runs its functions on threads when threading is
**      enabled and inline when it isn't; envMutex/envScopedLock keep
**      threads from interleaving.
**
**      Ported from TestLibXLT/Core/Env/envThreadTest.cpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef ENV_THREAD_HPP
#include "Core/env/envThread.hpp"
#endif
#ifndef ENV_THREADGROUP_HPP
#include "Core/env/envThreadGroup.hpp"
#endif

#include <gtest/gtest.h>

#include <atomic>
#include <string>
#include <thread>

namespace
{

//	Restores envThreadGroup's global threading switch after each test.
//
class envThreadGroupTest : public testing::Test
{
	protected:
		void SetUp() override { m_bWasEnabled = envThreadGroup::GetThreadingEnabled(); }
		void TearDown() override { envThreadGroup::SetThreadingEnabled(m_bWasEnabled); }

	private:
		bool m_bWasEnabled = false;
};

}

TEST_F(envThreadGroupTest, RunsInlineWhenThreadingIsDisabled)
{
	envThreadGroup::SetThreadingEnabled(false);

	const std::thread::id caller = std::this_thread::get_id();
	std::thread::id ran_on;
	bool ran = false;

	envThreadGroup threads;
	threads.AddThread([&]() { ran = true; ran_on = std::this_thread::get_id(); });

	// already done, before WaitForAll
	EXPECT_TRUE(ran);
	EXPECT_EQ(caller, ran_on);
	threads.WaitForAll();
}

TEST_F(envThreadGroupTest, WaitForAllWaitsForEveryThread)
{
	envThreadGroup::SetThreadingEnabled(true);

	std::atomic<int> done(0);
	envThreadGroup threads;
	for( int i = 0 ; i < 8 ; i++ )
	{
		threads.AddThread([&done]()
		{
			for( int j = 0 ; j < 10000 ; j++ )
				delete new char;
			++done;
		});
	}
	threads.WaitForAll();

	EXPECT_EQ(8, done.load());
}

//	Two threads each append 4 runs of 5 characters, holding the lock for
//	each run; the runs must come out whole.
//
TEST_F(envThreadGroupTest, ScopedLockKeepsRunsTogether)
{
	envThreadGroup::SetThreadingEnabled(true);

	std::string buffer;
	envMutex buffer_mutex;
	auto fill = [&](char i_First)
	{
		for( char ch = i_First ; ch < i_First + 4 ; ch++ )
		{
			envScopedLock lock(buffer_mutex);
			for( int i = 0 ; i < 5 ; i++ )
			{
				buffer += ch;
				std::this_thread::yield();
			}
		}
	};

	envThreadGroup threads;
	threads.AddThread([&]() { fill('a'); });
	threads.AddThread([&]() { fill('A'); });
	threads.WaitForAll();

	ASSERT_EQ(40u, buffer.size());
	for( size_t run = 0 ; run < buffer.size() ; run += 5 )
		EXPECT_EQ(std::string(5, buffer[run]), buffer.substr(run, 5)) << buffer;
}
