/*****************************************************************************
**  CoreTestsMain.cpp
**
**      Runs the LibXLT Core unit tests. Core is initialized once for the
**      whole run, and failed DBG_ASSERTs are reported as test failures
**      instead of opening the abort/continue dialog.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctPlatform.hpp"

#ifndef CORE_LAYER_HPP
#include "Core/CoreLayer.hpp"
#endif
#include "Core/dbg/dbgMsg.hpp"

#include <gtest/gtest.h>

#include <string>

namespace
{
	void ReportAssert(const std::string& i_Message)
	{
		ADD_FAILURE() << "DBG_ASSERT failed: " << i_Message;
	}
}

int main(int argc, char** argv)
{
	ctPlatform::SetUpUnattended();
	testing::InitGoogleTest(&argc, argv);

	CoreLayer::Init();
	dbgMsg::AssertHandler previous_handler = dbgMsg::SetAssertHandler(&ReportAssert);

	int result = RUN_ALL_TESTS();

	dbgMsg::SetAssertHandler(previous_handler);
	CoreLayer::CleanUp();
	return result;
}
