/*****************************************************************************
**  GraphicsTestsMain.cpp
**
**      Runs the LibXLT Graphics unit tests (code that needs no GPU). Core is initialized once for the
**      whole run, and failed DBG_ASSERTs are reported as test failures
**      instead of opening the abort/continue dialog.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtPlatform.hpp"

#ifndef CORE_LAYER_HPP
#include "Core/CoreLayer.hpp"
#endif
#include "Core/dbg/dbgMsg.hpp"
#ifndef GF_PATHS_HPP
#include "Core/Gf/gfPaths.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
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
	gtPlatform::SetUpUnattended();
	gtPlatform::StartCom();
	testing::InitGoogleTest(&argc, argv);

	CoreLayer::Init();
	dbgMsg::AssertHandler previous_handler = dbgMsg::SetAssertHandler(&ReportAssert);

	//	What the libraries keep in the user data folder (the Vulkan pipeline
	//	cache) goes to a temporary folder, not the real one; one per shard
	//	(RunShards.ps1 runs them at the same time)
	std::string user_data_name = "UserData";
	if (const char* shard = std::getenv("GTEST_SHARD_INDEX"))
	{
		user_data_name += std::string("-shard") + shard;
	}
	const std::filesystem::path user_data = std::filesystem::temp_directory_path() / "MspGraphicsTests" / user_data_name;
	std::filesystem::create_directories(user_data);
	fsLocator user_data_locator;
	fsFileUtil::UnicodeStringToLocator(user_data.u8string(), user_data_locator);
	gfPaths::ResetPath(gfPaths::e_UserDataPath, user_data_locator);

	int result = RUN_ALL_TESTS();

	dbgMsg::SetAssertHandler(previous_handler);
	CoreLayer::CleanUp();
	gtPlatform::StopCom();
	return result;
}
