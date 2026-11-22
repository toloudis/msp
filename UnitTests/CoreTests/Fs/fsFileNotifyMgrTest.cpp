/*****************************************************************************
**  fsFileNotifyMgrTest.cpp
**
**      fsFileNotifyMgr, MachStudio's "Enable Asset Update" preference: the
**      resource tracker reports the files a scene uses, and Check, called
**      every frame, reloads the textures that changed on disk, checking up
**      to 100 files every 5 seconds. Each test sets the files' modified
**      times itself, so nothing depends on the clock.
**
**	Extra Large Technology
\****************************************************************************/
#include "ctTempDir.hpp"

#ifndef FS_FILENOTIFYMGR_HPP
#include "Core/fs/private/fsFileNotifyMgr.hpp"
#endif

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace
{
	std::vector<std::string> g_Reloaded;

	void RecordReload(const fsLocator& i_Texture, bool)
	{
		g_Reloaded.push_back(i_Texture.GetLastName());
	}

	std::vector<std::string> TakeReloaded()
	{
		std::vector<std::string> reloaded;
		reloaded.swap(g_Reloaded);
		std::sort(reloaded.begin(), reloaded.end());
		return reloaded;
	}

	using Names = std::vector<std::string>;

	//	Watches files in a temp directory, each with its own modified time
	class Watcher
	{
		public:
			Watcher()
			{
				fsFileNotifyMgr::Intialize();
				g_Reloaded.clear();
				m_Mgr.TextureReloadFunction(&RecordReload);
			}

			~Watcher()
			{
				m_Mgr.TextureReloadFunction(nullptr);
				fsFileNotifyMgr::DeInitialize();
			}

			//	A file whose modified time is i_Hour hours after a fixed point
			fsLocator Add(const std::string& i_Name, int i_Hour)
			{
				const fsLocator file = m_Dir.GetLocator(i_Name.c_str());
				std::ofstream(m_Dir.GetPath() / i_Name) << "x";
				SetHour(i_Name, i_Hour);
				m_Mgr.FileAdded(file);
				return file;
			}

			void SetHour(const std::string& i_Name, int i_Hour)
			{
				std::filesystem::last_write_time(m_Dir.GetPath() / i_Name, c_Base + std::chrono::hours(i_Hour));
			}

			void Remove(const fsLocator& i_File) { m_Mgr.FileRemoved(i_File); }

		private:
			const std::filesystem::file_time_type c_Base =
				std::filesystem::file_time_type::clock::now() - std::chrono::hours(24 * 30);

			ctTempDir m_Dir;
			fsFileNotifyMgr m_Mgr;
	};

	std::string Numbered(int i_Number)
	{
		return "tex" + std::to_string(1000 + i_Number) + ".png";
	}
}

TEST(fsFileNotifyMgr, NothingChangedReloadsNothing)
{
	Watcher watcher;
	watcher.Add("a.png", 1);
	watcher.Add("b.png", 2);
	watcher.Add("c.png", 3);

	fsFileNotifyMgr::Check(1000);
	EXPECT_EQ(Names{}, TakeReloaded());
}

TEST(fsFileNotifyMgr, AChangedFileIsReloaded)
{
	Watcher watcher;
	watcher.Add("a.png", 1);
	watcher.Add("b.png", 2);
	watcher.Add("c.png", 3);

	// the first file as well as the others
	watcher.SetHour("a.png", 100);
	watcher.SetHour("c.png", 100);
	fsFileNotifyMgr::Check(1000);
	EXPECT_EQ((Names{"a.png", "c.png"}), TakeReloaded());

	// and only once
	fsFileNotifyMgr::Check(2000);
	EXPECT_EQ(Names{}, TakeReloaded());
}

TEST(fsFileNotifyMgr, AfterAFileIsRemovedTheOthersKeepTheirTimes)
{
	Watcher watcher;
	const fsLocator a = watcher.Add("a.png", 1);
	watcher.Add("b.png", 2);
	watcher.Add("c.png", 3);
	watcher.Remove(a);

	fsFileNotifyMgr::Check(1000);
	EXPECT_EQ(Names{}, TakeReloaded()) << "nothing changed";

	watcher.SetHour("c.png", 100);
	fsFileNotifyMgr::Check(2000);
	EXPECT_EQ(Names{"c.png"}, TakeReloaded());
}

TEST(fsFileNotifyMgr, ChecksAtMostEveryFiveSeconds)
{
	Watcher watcher;
	watcher.Add("first.png", 0);	// so this doesn't depend on the first file
	watcher.Add("a.png", 1);
	fsFileNotifyMgr::Check(1000);

	watcher.SetHour("a.png", 100);
	fsFileNotifyMgr::Check(1003);
	EXPECT_EQ(Names{}, TakeReloaded()) << "too soon";
	fsFileNotifyMgr::Check(1006);
	EXPECT_EQ(Names{"a.png"}, TakeReloaded());
}

TEST(fsFileNotifyMgr, KeepsCheckingWhenTheSecondsWrap)
{
	// Check is given the time in seconds cut to 16 bits, so it wraps about
	// every 18 hours
	Watcher watcher;
	watcher.Add("first.png", 0);	// so this doesn't depend on the first file
	watcher.Add("a.png", 1);
	fsFileNotifyMgr::Check(65530);

	watcher.SetHour("a.png", 100);
	fsFileNotifyMgr::Check(10);	// 16 seconds later
	EXPECT_EQ(Names{"a.png"}, TakeReloaded());
}

TEST(fsFileNotifyMgr, EveryFileIsCheckedInBatchesOfAHundred)
{
	Watcher watcher;
	for (int i = 0; i < 250; ++i)
		watcher.Add(Numbered(i), i);

	watcher.SetHour(Numbered(5), 1000);
	watcher.SetHour(Numbered(150), 1000);
	watcher.SetHour(Numbered(249), 1000);

	// Called every frame, as MachStudio does: the frames in between, too
	// soon to check, don't move on to the next batch
	Names reloaded;
	unsigned short time = 1000;
	for (int check = 0; check < 3; ++check)
	{
		fsFileNotifyMgr::Check(time);
		for (int frame = 0; frame < 10; ++frame)
			fsFileNotifyMgr::Check(time + 1);
		const Names batch = TakeReloaded();
		reloaded.insert(reloaded.end(), batch.begin(), batch.end());
		time += 10;
	}
	std::sort(reloaded.begin(), reloaded.end());
	EXPECT_EQ((Names{Numbered(5), Numbered(150), Numbered(249)}), reloaded);
}
