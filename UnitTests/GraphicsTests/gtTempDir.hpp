/*****************************************************************************
**  gtTempDir.hpp
**
**      gtTempDir makes a fresh, empty directory under the system temp
**      directory for one test, and deletes it (with its contents) when it
**      goes out of scope.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <gtest/gtest.h>

#include <filesystem>
#include <string>
#include <system_error>

class gtTempDir
{
	public:
		//	The directory is named after the running test so leftovers from
		//	a crashed run are easy to trace.
		gtTempDir()
		{
			const testing::TestInfo* info = testing::UnitTest::GetInstance()->current_test_info();
			std::string name = info ? std::string(info->test_suite_name()) + "." + info->name() : "gtTempDir";

			m_Path = std::filesystem::temp_directory_path() / "MspGraphicsTests" / name;
			std::error_code ec;
			std::filesystem::remove_all(m_Path, ec);
			std::filesystem::create_directories(m_Path);
		}

		~gtTempDir()
		{
			std::error_code ec;
			std::filesystem::remove_all(m_Path, ec);
		}

		gtTempDir(const gtTempDir&) = delete;
		gtTempDir& operator = (const gtTempDir&) = delete;

		const std::filesystem::path& GetPath() const { return m_Path; }

		//	GetLocator returns the directory, or a file or subdirectory of
		//	it when i_Name is given.
		fsLocator GetLocator(const char* i_Name = nullptr) const
		{
			std::filesystem::path path = i_Name ? m_Path / i_Name : m_Path;
			fsLocator locator;
			fsFileUtil::UnicodeStringToLocator(path.u8string(), locator);
			return locator;
		}

	private:
		std::filesystem::path m_Path;
};
