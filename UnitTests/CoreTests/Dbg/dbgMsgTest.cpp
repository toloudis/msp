/*****************************************************************************
**  dbgMsgTest.cpp
**
**      dbgMsg output streams and their switches, and the assert handler
**      the test runner relies on.
**
**      Replaces TestLibXLT/Core/Dbg/dbgTestMain.cpp, which wrote to log
**      files for someone to read.
**
**	Extra Large Technology
\****************************************************************************/
#include "Core/dbg/dbgMsg.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <string>

namespace
{

//	Adds a dbgMsg stream that writes into a string for the length of a
//	test.
//
class dbgMsgStream : public testing::Test
{
	protected:
		void SetUp() override { dbgMsg::addStream(m_Name, &m_Output); }
		void TearDown() override { dbgMsg::removeStream(m_Name); }

		void Error(const std::string& i_Msg)
		{
			std::wstring file(L"dbgMsgTest.cpp");
			dbgMsg::Error(file, 1, i_Msg);
		}

		bool Wrote(const std::string& i_Msg) const
		{
			return m_Output.str().find(i_Msg) != std::string::npos;
		}

		std::wstring m_Name = L"CoreTestsStream";
		std::ostringstream m_Output;
};

std::string l_LastAssert;
int l_NumAsserts = 0;

void CaptureAssert(const std::string& i_Message)
{
	l_LastAssert = i_Message;
	++l_NumAsserts;
}

}

TEST_F(dbgMsgStream, ErrorsReachAnAddedStream)
{
	Error("first error");
	EXPECT_TRUE(Wrote("first error")) << m_Output.str();
}

TEST_F(dbgMsgStream, DisabledErrorsAreNotWritten)
{
	dbgMsg::enableStreamError(m_Name, false);
	Error("hidden error");
	EXPECT_FALSE(Wrote("hidden error")) << m_Output.str();

	dbgMsg::enableStreamError(m_Name, true);
	Error("shown error");
	EXPECT_TRUE(Wrote("shown error")) << m_Output.str();
}

TEST_F(dbgMsgStream, DisabledStreamIsNotWritten)
{
	dbgMsg::enableDbgStream(m_Name, false);
	Error("hidden error");
	EXPECT_FALSE(Wrote("hidden error")) << m_Output.str();

	dbgMsg::enableDbgStream(m_Name, true);
	Error("shown error");
	EXPECT_TRUE(Wrote("shown error")) << m_Output.str();
}

TEST_F(dbgMsgStream, RemovedStreamIsNotWritten)
{
	dbgMsg::removeStream(m_Name);
	Error("after removal");
	EXPECT_FALSE(Wrote("after removal")) << m_Output.str();

	// TearDown removes it again, which is harmless
}

//	DBG_ASSERT only exists in debug builds.
//
TEST(dbgMsgAssert, HandlerReceivesEachFailure)
{
#if ENV_DEBUG
	dbgMsg::AssertHandler previous = dbgMsg::SetAssertHandler(&CaptureAssert);
	l_NumAsserts = 0;

	DBG_ASSERT(1 + 1 == 2, "should not fire");
	EXPECT_EQ(0, l_NumAsserts);

	DBG_ASSERT(false, "first failure " << 42);
	DBG_ASSERT(false, "second failure");

	dbgMsg::SetAssertHandler(previous);

	EXPECT_EQ(2, l_NumAsserts);
	EXPECT_NE(std::string::npos, l_LastAssert.find("second failure")) << l_LastAssert;
	// each message stands alone
	EXPECT_EQ(std::string::npos, l_LastAssert.find("first failure")) << l_LastAssert;
	EXPECT_NE(std::string::npos, l_LastAssert.find("dbgMsgTest.cpp")) << l_LastAssert;
#else
	GTEST_SKIP() << "DBG_ASSERT is compiled out of release builds";
#endif
}
